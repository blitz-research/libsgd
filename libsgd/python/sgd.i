%module sgd

%begin %{
#ifdef _MSC_VER
#define SWIG_PYTHON_INTERPRETER_NO_DEBUG
#endif
%}

%{
#define SWIG_FILE_WITH_INIT
#include <sgd/sgd.h>
%}

%include "renames.i"

%include <sgd/sgd.h>
%include <sgd/keycodes.h>
