#if !__cplusplus
#error This file must only be included by c++ code
#endif

#pragma once

#include <core/exports.h>

#include "types.h"

namespace sgd {

SGD_SHARED(Window);
SGD_SHARED(Scene);

inline Window* mainWindow;

inline Scene* mainScene;

SGD_SHARED(Material);
SGD_SHARED(Mesh);
SGD_SHARED(Entity);

enum struct HandleType {
	material,
	mesh,
	entity,
};

template <class T> HandleType handleType();

//clang-format off
#define SGD_HANDLE_TYPE(C, H) template <> HandleType handleType<C>() { return H; }
//clang-format on

SGD_HANDLE_TYPE(Material, HandleType::material);
SGD_HANDLE_TYPE(Mesh, HandleType::mesh);
SGD_HANDLE_TYPE(Entity, HandleType::entity);

SGD_Handle getHandle(HandleType type, Shared* shared);

SGD_Handle createHandle(HandleType type, Shared* shared);

Shared* resolveHandle(HandleType type, SGD_Handle handle);

void releaseHandle(HandleType type, SGD_Handle handle);

template<class T> SGD_Handle getHandle(T* shared) {
	return getHandle(handleType<T>(), shared);
}

template<class T> SGD_Handle createHandle(T* shared) {
	return createHandle(handleType<T>(), shared);
}

template<class T> T* resolveHandle(SGD_Handle handle) {
	auto shared = resolveHandle(handleType<T>(), handle);
	SGD_ASSERT(!shared || shared->template as<T>());
	return shared ? shared->template as<T>() : nullptr;
}

template<class T> T* releaseHandle(SGD_Handle handle) {
	resoleaseHandle(handleType<T>(), handle);
}

}
