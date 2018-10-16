cuda_add_executable(check-cuda.exe check.cu)
target_link_libraries(check-cuda.exe fti.static ${MPI_C_LIBRARIES} m)
set_property(TARGET check-cuda.exe APPEND PROPERTY COMPILE_FLAGS ${MPI_C_COMPILE_FLAGS})
set_property(TARGET check-cuda.exe APPEND PROPERTY LINK_FLAGS ${MPI_C_LINK_FLAGS})

if(ENABLE_SIONLIB)
    set(IO 4)
    set(IO_NAMES "POSIX MPIIO FTIFF SIONLIB")
else()
    set(IO 3)
    set(IO_NAMES "POSIX MPIIO FTIFF")
endif()

set(DIR ${CMAKE_BINARY_DIR}/)

configure_file(run-checks.in ${CMAKE_CURRENT_SOURCE_DIR}/run-checks.sh @ONLY)

if(NOT $MPIRUN)
	set(MPIRUN mpirun)
endif()

add_custom_target(demo
	COMMAND awk '$$1=="failure" {$$3=0}1' config.fti > tmp 
	COMMAND mv tmp config.fti
	COMMAND ${MPIRUN} -n 4 ./check-cuda.exe config.fti 1 1 1
	COMMAND ${MPIRUN} -n 4 ./check-cuda.exe config.fti 0 1 1
)

install(DIRECTORY DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Local)
install(DIRECTORY DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Global)
install(DIRECTORY DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Meta)
install(DIRECTORY DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/chk)

file(COPY Global Local Meta chk DESTINATION .)
file(COPY config.fti DESTINATION .)

file(INSTALL run-checks.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR} PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE)

file(REMOVE run-checks.sh)
file(REMOVE check_functions.mod)
