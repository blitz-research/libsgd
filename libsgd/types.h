#ifndef SGD_TYPES_H_INCLUDED
#define SGD_TYPES_H_INCLUDED

#include "beginextern.h"

#define SGD_API

typedef int SGD_Bool;
typedef int SGD_Handle;
typedef const char* SGD_String;

SGD_API void sgd_Release(SGD_Handle handle);

#include "endextern.h"

#endif
