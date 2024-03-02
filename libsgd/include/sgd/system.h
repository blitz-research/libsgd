#ifndef SGD_SYSTEM_H_INCLUDED
#define SGD_SYSTEM_H_INCLUDED

#include "types.h"

SGD_API void SGD_DECL sgd_SetErrorHandler(void(SGD_DECL *handler)(const char* error, void* context), void* context);

SGD_API void SGD_DECL sgd_Run(void(SGD_DECL* start)());

SGD_API void SGD_DECL sgd_Error(SGD_String error);

#endif

