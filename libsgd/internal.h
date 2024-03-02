#pragma once

#include <graphics/exports.h>
#include <window/exports.h>

#include <sgd/sgd.h>

namespace sgd {

inline WindowPtr mainWindow;
inline GraphicsContextPtr mainGC;
inline ScenePtr mainScene;

enum struct HandleType {
	object,
	texture,
	material,
	mesh,
	entity,
};

template<class T> const char *handleTypeName();
template<class T> HandleType handleType();

// clang-format off
#define SGD_TYPE_INFO(T, H) \
template<> inline const char* handleTypeName<T>() { return #T; } \
template<> inline HandleType handleType<T>() { return H; }
// clang-format on

SGD_TYPE_INFO(Texture, HandleType::texture);
SGD_TYPE_INFO(Material, HandleType::material);
SGD_TYPE_INFO(Mesh, HandleType::mesh);
SGD_TYPE_INFO(Entity, HandleType::entity);
SGD_TYPE_INFO(Camera, HandleType::entity);
SGD_TYPE_INFO(Light, HandleType::entity);
SGD_TYPE_INFO(Model, HandleType::entity);
SGD_TYPE_INFO(Skybox, HandleType::entity);

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
	if(!shared) sgd_Error((sgd::String("Invalid ") + handleTypeName<T>() + " handle").c_str());
	return shared->template as<T>();
}

template<class T> T* releaseHandle(SGD_Handle handle) {
	releaseHandle(handleType<T>(), handle);
}

void error(CString message);

void error(CString error, CString message);

void error(CString error, CFileioEx& ex);

}
