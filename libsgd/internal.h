#if __cplusplus

#pragma once

#include "types.h"

#include <window/exports.h>

namespace sgd {

inline sgd::Window* mainWindow;

SGD_Handle createHandle(sgd::Shared* object);
void destroyHandle(SGD_Handle handle);

SGD_Handle getHandle(sgd::Shared* object);
sgd::Shared* getObject(SGD_Handle handle);

template <class T> T* getObject(SGD_Handle handle) {
	return getObject(handle)->to<T>();
}

}

#endif
