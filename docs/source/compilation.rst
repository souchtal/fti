.. Fault Tolerance Library documentation Compilation file


.. Compilation
.. ===================================================

FTI uses Cmake to configure the installation. The recommended way to perform the installation is to create a build directory within the base directory of FTI and perform the cmake command in there. In the following you will find configuration examples. The commands are performed in the build directory within the FTI base directory.

Default The default configuration builds the FTI library with Fortran and MPI-IO support for GNU compilers:

.. code-block:: cmake

   cmake -DCMAKE_INSTALL_PREFIX:PATH=/install/here/fti ..
   make all install