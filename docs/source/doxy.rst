.. Fault Tolerance Library documentation Doxy file
.. _doxy:

Doxygen Documentation
==============================

For a deeper dive into FTI's members, you can explore the documentation through Doxygen's auto-generated page_.


.. _page: http://leobago.github.io/fti/

Contributing 
-------------------------------

..
	Upon contributing to FTI with a new API, it is recommended to have it show on the APIs pag. To do so, add the following lines ``FTI_ROOT/docs/source/apireferences.rst`` where ``FTI_API`` is the name of your function.

.. note::
	Notice: For further details on how to include Doxygen's directives to ReadTheDocs page, visit the Breathe's guide_. 

.. _guide: https://breathe.readthedocs.io/en/latest/directives.html

.. code-block::
	
	.. doxygenfunction:: FTI_API
	:project: Fault Tolerance Library 

	