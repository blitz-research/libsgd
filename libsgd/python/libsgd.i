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

%rename("%(strip:[sgd_])s") "";

void sgd_CreateWindow(int x, int y, const char* title, int flags);

