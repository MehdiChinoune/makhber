#[[
FindPyQt5
-------

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``PyQt5_FOUND``
  True if the system has PyQt5 sip files.
``PyQt5_INCLUDE_DIRS``
  The directory containing PyQt5 sip files.
``PyQt5_FLAGS``
  The required flags to compile generated c++ files.
``PyQt5_SIP``
  whether PyQt5 include the sip module or not. (PyQt5*.sip)

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``PyQt5_INCLUDE_DIR``
  The directory containing PyQt5 sip files.
]]

execute_process(
  COMMAND ${Python3_EXECUTABLE} -c "import sys; print(sys.prefix)"
  OUTPUT_VARIABLE _Python3_PREFIX
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

execute_process(
  COMMAND ${Python3_EXECUTABLE} -c "import PyQt5; print(PyQt5.__path__[0])"
  OUTPUT_VARIABLE _PyQt5_PATH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

set( _PyQt5_HINTS
  "${_Python3_PREFIX}/share/sip" # Ubuntu<21.04 & OpenSUSE
  "${Python3_SITELIB}"
  "${Python3_SITEARCH}"
  )

find_path( PyQt5_INCLUDE_DIR
  NAMES "QtCore/QtCoremod.sip"
  PATHS "${_PyQt5_PATH}"
  HINTS ${_PyQt5_HINTS}
  PATH_SUFFIXES "PyQt5" "PyQt5/bindings" "bindings"
  )

execute_process(
  COMMAND ${Python3_EXECUTABLE} -c "from PyQt5.QtCore import PYQT_VERSION_STR; print(PYQT_VERSION_STR)"
  OUTPUT_VARIABLE PyQt5_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND ${Python3_EXECUTABLE} -c "import PyQt5.sip"
  RESULT_VARIABLE _No_PyQt5_SIP
  ERROR_QUIET
)
# An error returns 1 which considered by cmake as true
if( _No_PyQt5_SIP )
  set( PyQt5_SIP OFF )
else()
  set( PyQt5_SIP ON )
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args( PyQt5
  REQUIRED_VARS PyQt5_INCLUDE_DIR
  VERSION_VAR PyQt5_VERSION
)

if( PyQt5_FOUND )
  set( PyQt5_INCLUDE_DIRS ${PyQt5_INCLUDE_DIR} )
endif()

mark_as_advanced( PyQt5_INCLUDE_DIR )
