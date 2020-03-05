# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os, subprocess
import sys
import sphinx_rtd_theme
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'Fault Tolerance Library'
copyright = '2020, Leonardo Bautista-Gomez'
author = 'Leonardo Bautista-Gomez'

# The full version, including alpha/beta/rc tags
release = '1.4'


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = ['breathe']

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'
html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

#everyting below is customization
breathe_default_project = "Fault Tolerance Library"
breathe_projects= {"Fault Tolerance Library": "../../doc/Doxygen/xml"}

#enable when building on ReadTheDocs server
#read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'
read_the_docs_build = os.environ.get('READTHEDOCS') == 'True'
if read_the_docs_build:
	#print ("We are in ReadTheDocs server")
	subprocess.call('cd ../../doc/Doxygen; doxygen Doxyfile.in', shell=True)