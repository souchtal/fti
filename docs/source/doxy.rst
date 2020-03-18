.. Fault Tolerance Library documentation Doxy file
.. _doxy:

Doxygen Documentation
==============================

For a deeper dive into FTI's members, you can explore the documentation through Doxygen's auto-generated page_.


.. _page: http://leobago.github.io/fti/

Contributing 
-------------------------------

FTI uses Doxygen_ for generating its documentation. For a new function to be included in the documentation, it should be commented in the source code as follows:


.. code-block:: C
	/*-------------------------------------------------------------------------*/
	/**
	  @brief      It sets/resets the pointer and type to a protected variable.
	  @param      id              ID for searches and update.
	  @param      ptr             Pointer to the data structure.
	  @param      count           Number of elements in the data structure.
	  @param      type            Type of elements in the data structure.
	  @return     integer         FTI_SCES if successful.

	  This function stores a pointer to a data structure, its size, its ID,
	  its number of elements and the type of the elements. This list of
	  structures is the data that will be stored during a checkpoint and
	  loaded during a recovery. It resets the pointer to a data structure,
	  its size, its number of elements and the type of the elements if the
	  dataset was already previously registered.

	 **/
	/*-------------------------------------------------------------------------*/
	int FTI_Protect(int id, void* ptr, long count, FTIT_type type)
	{
	...
	}



Upon contributing to FTI with a new API, it is recommended to have it show on the APIs pag. To do so, add the following lines ``FTI_ROOT/docs/source/apireferences.rst`` where ``FTI_API`` is the name of your function.


.. code-block::
	
	.. doxygenfunction:: FTI_API
	:project: Fault Tolerance Library 


.. _guide: https://breathe.readthedocs.io/en/latest/directives.html
.. _Doxygen: http://www.doxygen.nl/

For further details on how to include Doxygen's directives to ReadTheDocs page, visit the Breathe's guide_. 