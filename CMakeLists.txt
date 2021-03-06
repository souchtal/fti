cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

if(POLICY CMP0054)
	cmake_policy(SET CMP0054 NEW)
endif()

if (ENABLE_GPU)
  if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    if( ${ENABLE_FORTRAN} )
      project(FTI C Fortran CUDA CXX)
    else()
      project(FTI C CUDA CXX)
    endif()
  endif()
  FIND_PACKAGE(CUDA)
else ()
  if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
      if( ${ENABLE_FORTRAN} )
        project(FTI C Fortran)
      else()
        project(FTI C)
      endif()
  endif()
endif()  


## [START] DETERMINE COMPILER MAJOR VERSIONS
string(REGEX MATCH "[0-9]+" CMAKE_C_COMPILER_VERSION_MAJOR ${CMAKE_C_COMPILER_VERSION} )
set(FORTRAN_VER_UNKNOWN 0)
set(ADD_CFLAGS "")

if( ${ENABLE_FORTRAN} )
    if (NOT CMAKE_Fortran_COMPILER_VERSION)

        message(AUTHOR_WARNING "
        ** Cmake variable 'CMAKE_Fortran_COMPILER_VERSION' is unset
        *  attempt to determine it manually...")

        string(COMPARE EQUAL ${CMAKE_Fortran_COMPILER_ID} "Intel" COMPILER_IDENT_INTEL)
        string(COMPARE EQUAL ${CMAKE_Fortran_COMPILER_ID} "GNU" COMPILER_IDENT_GNU)
        string(COMPARE EQUAL ${CMAKE_Fortran_COMPILER_ID} "PGI" COMPILER_IDENT_PGI)
        if(${COMPILER_IDENT_INTEL})
            set(VER_CHECK_SRC "${CMAKE_SOURCE_DIR}/testing/cmake/intel_major_ver.f90")
        elseif(${COMPILER_IDENT_GNU})
            set(VER_CHECK_SRC "${CMAKE_SOURCE_DIR}/testing/cmake/gnu_major_ver.f90")
        elseif(${COMPILER_IDENT_PGI})
            set(VER_CHECK_SRC "${CMAKE_SOURCE_DIR}/testing/cmake/pgi_major_ver.f90")
        endif()

        set(CMAKE_Fortran_FLAGS "-cpp")
        try_run( PROG_RAN COMPILE_SUCCESS
            "${CMAKE_BINARY_DIR}" "${VER_CHECK_SRC}"
            RUN_OUTPUT_VARIABLE VER_STRING
            )
        if ( COMPILE_SUCCESS )
            string( REGEX MATCH "[0-9]+"
                DETECTED_VER "${VER_STRING}"
                )

            set( CMAKE_Fortran_COMPILER_VERSION_MAJOR "${DETECTED_VER}" )

            message(AUTHOR_WARNING "
            ** The major version was determined as: ${VER_STRING}")
        else()

            set(FORTRAN_VER_UNKNOWN 1)
            set(CMAKE_Fortran_COMPILER_VERSION_MAJOR "")
            message(AUTHOR_WARNING "
            ** The Fortran version could not be determined!")

        endif()
    else()

        string(REGEX MATCH "[0-9]+" CMAKE_Fortran_COMPILER_VERSION_MAJOR ${CMAKE_Fortran_COMPILER_VERSION} )

    endif()

    if(NOT ${FORTRAN_VER_UNKNOWN})

        set(C_COMPILER_IDENT "${CMAKE_C_COMPILER_ID}_${CMAKE_C_COMPILER_VERSION_MAJOR}")
        set(Fortran_COMPILER_IDENT "${CMAKE_Fortran_COMPILER_ID}_${CMAKE_Fortran_COMPILER_VERSION_MAJOR}")
        string(COMPARE NOTEQUAL ${C_COMPILER_IDENT} ${Fortran_COMPILER_IDENT} COMPILER_VER_DIFFER)
        if(${COMPILER_VER_DIFFER})
            message(WARNING "
            ** You are using different compiler idetifications for Fortran and C!
            *  This might lead to undefined behavior!")
        endif()

    endif()
endif()
## [END] DETERMINE COMPILER MAJOR VERSIONS

option(ENABLE_FORTRAN "Enables the generation of the Fortran wrapper for FTI" OFF)
option(ENABLE_EXAMPLES "Enables the generation of examples" ON)
option(ENABLE_SIONLIB "Enables the parallel I/O SIONlib for FTI" OFF)
option(ENABLE_HDF5 "Enables the HDF5 checkpoints for FTI" OFF)
option(ENABLE_TESTS "Enables the generation of tests" ON)
option(ENABLE_FI_IO "Enables the I/O failure injection mechanism" OFF)
option(ENABLE_LUSTRE "Enables Lustre Support" OFF)
option(ENABLE_DOCU "Enables the generation of a Doxygen documentation" OFF)
option(ENABLE_TUTORIAL "Enables the generation of tutorial files" OFF)
option(ENABLE_IME_NATIVE "Enables the IME native API" OFF)

set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS ON)

include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeScripts")
include(AppendProperty)
if(${ENABLE_FORTRAN})
    include(FortranCInterface)
    include(CheckFortranCompilerFlag)
endif()

include(CheckCCompilerFlag)

#check for ZLIB (crc32)
find_package(ZLIB)
if(NOT ZLIB_FOUND)
    set(ADD_CFLAGS "${ADD_CFLAGS} -DFTI_NOZLIB")
endif()

find_package(MPI REQUIRED)
if(NOT DEFINED NO_OPENSSL)
	find_package(OPENSSL REQUIRED)
	if("${OPENSSL_FOUND}")
		include_directories( ${OPENSSL_INCLUDE_DIR} )
	endif()
else()
	set(OPENSSL_FOUND false)
endif()

if(ENABLE_LUSTRE)
    find_package(LUSTREAPI)
endif()

add_subdirectory(src/deps)

include_directories("${CMAKE_CURRENT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include"
    ${MPI_Fortran_INCLUDE_PATH} ${MPI_C_INCLUDE_PATH})

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)

set(
SRC_FTI
    src/fortran/ftif.c
    src/util/tools.c
    src/util/utility.c
    src/util/keymap.c
    src/util/dataset.c
    src/util/ini.c
    src/util/macros.c
    src/util/failure-injection.c
    src/util/metaqueue.c
    src/IO/posix-dcp.c
    src/IO/hdf5-fti.c
    src/IO/ftiff.c
    src/IO/mpio.c
    src/IO/posix.c
    src/IO/ftiff-dcp.c
    src/postckpt.c
    src/conf.c
    src/fti-io.c
    src/recover.c
    src/postreco.c
    src/api-cuda.c
    src/api.c
    src/checkpoint.c
    src/dcp.c
    src/stage.c
    src/meta.c
    src/icp.c
    src/topo.c
)

if (ENABLE_SIONLIB)
    list(APPEND SRC_FTI "src/IO/sion-fti.c")
    message("${SRC_FTI}")
endif()

if (ENABLE_IME_NATIVE)
    list(APPEND SRC_FTI "src/IO/ime.c")
    message("${SRC_FTI}")
endif()

if (ENABLE_GPU)
set(
    SRC_CUDA_FTI
    src/IO/cuda-md5/md5Opt.cu
)
else ()
set(
    SRC_CUDA_FTI
    src/IO/cuda-md5/md5Opt.c
)
endif()


if (ENABLE_GPU)
  include_directories(${CUDA_INCLUDE_DIRS})
  set(ADD_CFLAGS "${ADD_CFLAGS} -DGPUSUPPORT")
  set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -DGPUSUPPORT")
  message("${CUDA_NVCC_FLAGS}")
  message("${ADD_CFLAGS}")
endif()


# add compiler flags
if("${CMAKE_C_COMPILER_ID}" STREQUAL "Intel")
    set(ADD_CFLAGS "${ADD_CFLAGS} -Wall")
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set(ADD_CFLAGS "${ADD_CFLAGS} -Wall")
  try_compile(C_COMPILER_HAS_FLAG ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/testing/cmake/minimal.c COMPILE_DEFINITIONS "-Wno-format-truncation")
  if(C_COMPILER_HAS_FLAG)
      set(ADD_CFLAGS "${ADD_CFLAGS} -Wno-format-truncation")
  endif()
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "PGI")
  try_compile(C_COMPILER_HAS_FLAG ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/testing/cmake/minimal.c COMPILE_DEFINITIONS "-Minform=inform")
  if(C_COMPILER_HAS_FLAG)
      set(ADD_CFLAGS "${ADD_CFLAGS} -Minform=inform")
  endif()
endif()

set(ADD_CFLAGS "${ADD_CFLAGS} -D_FILE_OFFSET_BITS=64")

if(${OPENSSL_FOUND})
	add_library(fti.static STATIC ${SRC_FTI} ${SRC_CUDA_FTI} ${OPENSSL_LIBRARIES}
    		$<TARGET_OBJECTS:iniparser> $<TARGET_OBJECTS:jerasure>)
	add_library(fti.shared SHARED ${SRC_FTI} ${SRC_CUDA_FTI} ${OPENSSL_LIBRARIES}
    		$<TARGET_OBJECTS:iniparser> $<TARGET_OBJECTS:jerasure>)
    set(HAVE_OPENSSL 1)
    #set(ADD_CFLAGS "${ADD_CFLAGS} -DHAVE_OPENSSL")
else()
	add_library(fti.static STATIC ${SRC_FTI} ${SRC_CUDA_FTI}
	    $<TARGET_OBJECTS:iniparser> $<TARGET_OBJECTS:jerasure>
		$<TARGET_OBJECTS:md5>)
	add_library(fti.shared SHARED ${SRC_FTI} ${SRC_CUDA_FTI}
	    $<TARGET_OBJECTS:iniparser> $<TARGET_OBJECTS:jerasure>
		$<TARGET_OBJECTS:md5>)
	append_property(TARGET fti.static fti.shared
		PROPERTY LINK_FLAGS "-DMD5P=TRUE")
	unset(OPENSSL_LIBRARIES)
    set(HAVE_OPENSSL 0)
endif()
set_target_properties(fti.static fti.shared PROPERTIES POSITION_INDEPENDENT_CODE True)

if( NOT ZLIB_FOUND )
append_property(SOURCE ${SRC_CUDA_FTI}
    PROPERTY COMPILE_FLAGS "-DFTI_NOZLIB")
else()
append_property(SOURCE ${SRC_CUDA_FTI}
    PROPERTY COMPILE_FLAGS )
endif()

append_property(TARGET fti.static fti.shared
	PROPERTY LINK_FLAGS "${MPI_C_LINK_FLAGS}")
set_property(TARGET fti.static fti.shared
	PROPERTY OUTPUT_NAME fti)

#PGCC C and C++ use builtin math functions, which are much more efficient than library calls.
#http://www.cecalc.ula.ve/documentacion/tutoriales/HPF/pgiws_ug/pgi30u09.htm
if(NOT "$ENV{COMPILER}" STREQUAL "pgi")
	find_library(LIBM m DOC "The math library")
endif()

if(ZLIB_FOUND)
    target_link_libraries(fti.static ${MPI_C_LIBRARIES} "${LIBM}" "${OPENSSL_LIBRARIES}" "${ZLIB_LIBRARIES}" ${CUDA_LIBRARIES})
    target_link_libraries(fti.shared ${MPI_C_LIBRARIES} "${LIBM}" "${OPENSSL_LIBRARIES}" "${ZLIB_LIBRARIES}" ${CUDA_LIBRARIES})
else()
    target_link_libraries(fti.static ${MPI_C_LIBRARIES} "${LIBM}" "${OPENSSL_LIBRARIES}" ${CUDA_LIBRARIES})
    target_link_libraries(fti.shared ${MPI_C_LIBRARIES} "${LIBM}" "${OPENSSL_LIBRARIES}" ${CUDA_LIBRARIES})
endif()

if(ENABLE_LUSTRE)
    if(LUSTREAPI_FOUND)
        include_directories(${LUSTREAPI_INCLUDE_DIRS})
        append_property(SOURCE ${SRC_FTI} PROPERTY COMPILE_DEFINITIONS LUSTRE)
        target_link_libraries(fti.static ${LUSTREAPI_LIBRARIES})
        target_link_libraries(fti.shared ${LUSTREAPI_LIBRARIES})
    else()
        message(WARNING "
  ** Lustre could not be found!
  *  You may specify:
  *  -DLUSTREAPI_CMAKE_LIBRARY_DIRS:PATH=<path to liblustreapi.a>
  *  and
  *  -DLUSTREAPI_CMAKE_INCLUDE_DIRS:PATH=<path to liblustreapi.h>")
    endif()
endif()


if(ENABLE_SIONLIB)
    set(SIONLIBBASE "" CACHE FILEPATH "base path to SIONlib installation")
    set(SIONLIB_INCLUDE_DIR "${SIONLIBBASE}/include/")
    include_directories("${SIONLIB_INCLUDE_DIR}")
    set(SIONLIB_CFLAGS "-I${SIONLIB_INCLUDE_DIR} -DSION_DEBUG -D_SION_LINUX  -DSION_MPI")
    find_library(SIONLIB_MPI NAMES "sionmpi_64" PATHS ${SIONLIBBASE} PATH_SUFFIXES "lib" NO_DEFAULT_PATH)
    find_library(SIONLIB_GEN NAMES "siongen_64" PATHS ${SIONLIBBASE} PATH_SUFFIXES "lib" NO_DEFAULT_PATH)
    find_library(SIONLIB_SER NAMES "sionser_64" PATHS ${SIONLIBBASE} PATH_SUFFIXES "lib" NO_DEFAULT_PATH)
    find_library(SIONLIB_COM NAMES "sioncom_64" PATHS ${SIONLIBBASE} PATH_SUFFIXES "lib" NO_DEFAULT_PATH)
    find_library(SIONLIB_COM_LOCK NAMES "sioncom_64_lock_none" PATHS ${SIONLIBBASE} PATH_SUFFIXES "lib" NO_DEFAULT_PATH)
    target_link_libraries(fti.static "${SIONLIB_MPI}" "${SIONLIB_GEN}" "${SIONLIB_SER}" "${SIONLIB_COM}" "${SIONLIB_COM_LOCK}")
    target_link_libraries(fti.shared "${SIONLIB_MPI}" "${SIONLIB_GEN}" "${SIONLIB_SER}" "${SIONLIB_COM}" "${SIONLIB_COM_LOCK}")
    add_definitions(-DENABLE_SIONLIB)
endif()

if(ENABLE_IME_NATIVE)
    if (IMELIBBASE)
        set(IMELIBBASE "" CACHE FILEPATH "base path to IME client lib installation")
    else()
        set(IMELIBBASE "/opt/ddn/ime/" CACHE FILEPATH "base path to IME client lib installation" FORCE)
    endif()
    set(IMELIB_INCLUDE_DIR "${IMELIBBASE}/include/")
    include_directories("${IMELIB_INCLUDE_DIR}")
    set(IMELIB_CFLAGS "-I${IMELIB_INCLUDE_DIR}")
    find_library(IMELIB NAMES "im_client" PATHS ${IMELIBBASE} PATH_SUFFIXES "lib" NO_DEFAULT_PATH)
    target_link_libraries(fti.static "${IMELIB}")
    target_link_libraries(fti.shared "${IMELIB}")
    add_definitions(-DENABLE_IME_NATIVE)
endif()


if(ENABLE_HDF5)
    find_package(HDF5 COMPONENTS HL C REQUIRED)
	if(HDF5_FOUND)
		add_definitions(-DENABLE_HDF5)
		include_directories(${HDF5_INCLUDE_DIRS})
		append_property(SOURCE ${SRC_FTI} PROPERTY COMPILE_DEFINITIONS HDF5)
		target_link_libraries(fti.static ${HDF5_LIBRARIES})
		target_link_libraries(fti.shared ${HDF5_LIBRARIES})
		target_link_libraries(fti.static ${HDF5_HL_LIBRARIES})
		target_link_libraries(fti.shared ${HDF5_HL_LIBRARIES})
	else()
		message(WARNING "
		** HDF5 could not be found!
		*  You may specify:
		*  -DHDF5_ROOT as the path to the HDF5 installation to use.
		**")
	endif()
endif()

if(ENABLE_FI_IO)
    add_definitions(-DENABLE_FTI_FI_IO)
endif()

append_property(SOURCE ${SRC_FTI}
    PROPERTY COMPILE_FLAGS "${MPI_C_COMPILE_FLAGS} ${SIONLIB_CFLAGS} ${ADD_CFLAGS}")

install(TARGETS fti.static fti.shared DESTINATION "${CMAKE_INSTALL_LIBDIR}" EXPORT FTI_EXPORT
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(FILES
    "include/fti.h"
    "include/fti-intern.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
configure_file(src/deps/md5/md5.h.in "${CMAKE_CURRENT_SOURCE_DIR}/src/deps/md5/md5.h" @ONLY)

if(ENABLE_FORTRAN)
    CHECK_Fortran_COMPILER_FLAG("-cpp" FC_HAS_CPP_FLAG)
    if(NOT FC_HAS_CPP_FLAG)
        message(FATAL_ERROR "
  ** Fortran compiler does not support -cpp flag.
  *  Please use a newer version!")
    endif()
    try_compile(MPI_USE_MOD ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/testing/cmake/checkMPImodule.f90
	    LINK_LIBRARIES ${MPI_Fortran_LIBRARIES}
	    CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${MPI_Fortran_INCLUDE_PATH}")
	if(NOT MPI_USE_MOD)
	    message(WARNING "
  ** mpi.mod was generated by a different compiler/compiler-version
  *  The Fortran examples and tests will be build using 'include mpif.h'!")

	endif()

    add_subdirectory(src/deps/bpp-0.3.0/ bpp/ EXCLUDE_FROM_ALL)

	bpp_preprocess(BPP_FTI_F90
		src/fortran/interface.F90.bpp)

	add_custom_target(bpp_file DEPENDS "${BPP_FTI_F90}") # to serialize src generation

	set(SRC_FTI_F90 ${BPP_FTI_F90}
		src/fortran/ftif.c)
	append_property(SOURCE ${SRC_FTI_F90}
		PROPERTY COMPILE_FLAGS "${MPI_Fortran_COMPILE_FLAGS}")

	add_library(fti_f90.static STATIC ${SRC_FTI_F90})
	add_dependencies(fti_f90.static bpp_file) # to serialize src generation
	target_link_libraries(fti_f90.static
		fti.static ${MPI_Fortran_LIBRARIES} ${MPI_C_LIBRARIES})
	set_property(TARGET fti_f90.static PROPERTY Fortran_MODULE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include")

	add_library(fti_f90.shared SHARED ${SRC_FTI_F90})
	add_dependencies(fti_f90.shared bpp_file) # to serialize src generation
	add_dependencies(fti_f90.shared fti_f90.static) # to serialize mod generation
	target_link_libraries(fti_f90.shared
		fti.shared ${MPI_Fortran_LIBRARIES} ${MPI_C_LIBRARIES})

	append_property(TARGET fti_f90.static fti_f90.shared
		PROPERTY LINK_FLAGS "${MPI_Fortran_LINK_FLAGS} ${MPI_C_LINK_FLAGS}")
	set_property(TARGET fti_f90.static fti_f90.shared
		PROPERTY OUTPUT_NAME fti_f90)

    install(TARGETS fti_f90.static fti_f90.shared EXPORT FTI_EXPORT
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
	install(FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/fti.mod
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
	set_target_properties(fti_f90.static fti_f90.shared PROPERTIES POSITION_INDEPENDENT_CODE True)
endif()

if(ENABLE_EXAMPLES)
	add_subdirectory(examples)
endif()

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(testing)
endif()

if(ENABLE_TUTORIAL)
	add_subdirectory(tutorial)
endif()

if(ENABLE_DOCU)
    add_subdirectory(doc/Doxygen)
endif()


# Installable config

include(CMakePackageConfigHelpers)
set(INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/FTI/cmake" CACHE PATH "cmake modules directory (DATADIR/FTI/cmake)")
write_basic_package_version_file("${CMAKE_BINARY_DIR}/FTIConfigVersion.cmake"
		VERSION "1.3"
		COMPATIBILITY AnyNewerVersion
)
install(EXPORT FTI_EXPORT DESTINATION "${INSTALL_CMAKEDIR}" FILE "FTILib.cmake")
install(FILES 
	"${CMAKE_BINARY_DIR}/FTIConfigVersion.cmake"
	CMakeScripts/FTIConfig.cmake
	DESTINATION "${INSTALL_CMAKEDIR}"
)


# in tree config (most likely useless...)

export(EXPORT FTI_EXPORT FILE "${PROJECT_BINARY_DIR}/FTILib.cmake")
configure_file(CMakeScripts/FTIConfig.cmake "${PROJECT_BINARY_DIR}/FTIConfig.cmake" COPYONLY)


# set up examples in build/examples/

file(COPY examples/config.fti DESTINATION examples)
file(COPY examples/configBkp.fti DESTINATION examples)
file(COPY examples/plot.sh DESTINATION examples)
file(COPY examples/vplot.plg DESTINATION examples)
file(COPY examples/README DESTINATION examples)
