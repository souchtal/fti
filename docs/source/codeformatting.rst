.. Fault Tolerance Library documentation Code Formatting file
.. _codeformatting:

FTI Code Format
======================


Code Checkers
----------------------

To enhance the code quality of FTI, we use the following open source code checkers:

.. list-table::
   :header-rows: 1

   * - Language
     - Code Checker
   * - C
     - cpplint
   * - Fortran
     - fprettify
   * - CMake
     - cmakelint


Implementation
----------------------

Code checking is integrated in FTI through a script that traverses any added/modified code in FTI and checks if it conforms to the desired coding style. The script acts as a pre-commit hook that gets fired by a local commit. 

Examples of the execution on FTI s code


Contributing
----------------------


..

	To make use of the pre-commit hook, after cloning the repository, one should initialize their branch through ``git init`` command.


.. note::
	Notice: For a temporary commit where the developer is aware that the code might still need formatting but still wants to commit, use the flag **--no-verify**
