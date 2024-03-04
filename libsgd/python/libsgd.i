%module libsgd

%begin %{
#ifdef _MSC_VER
#define SWIG_PYTHON_INTERPRETER_NO_DEBUG
#endif
%}

%{
#define SWIG_FILE_WITH_INIT
#include "../include/sgd/sgd.h"
%}

// This removes sgd_ prefix
%rename("%(strip:[sgd_])s") "";

%include "../include/sgd/sgd.h"
