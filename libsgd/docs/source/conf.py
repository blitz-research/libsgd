# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'LibSGD'
copyright = '2024, Blitz Research'
author = 'Blitz Research'
release = 'V0.15'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ['breathe','sphinx_tabs.tabs']

templates_path = ['_templates']
exclude_patterns = []

# breathe config
#
breathe_projects = {
    "LibSGD": "${CMAKE_CURRENT_BINARY_DIR}/xml" # Should be same as Doxyfile output directory
}
breathe_default_project = "LibSGD"

breathe_show_enumvalue_initializer = True

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'nature'
html_theme_options = {
	"sidebarwidth": "320"
}

html_static_path = ['_static']
html_css_files = ["custom.css"]
html_show_sourcelink = False
