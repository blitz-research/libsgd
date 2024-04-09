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

// This removes sgd_ SGD_ prefix
%rename("%(regex:/(sgd_|SGD_)(.*)/\\2/)s") "";

%include <sgd/sgd.h>
