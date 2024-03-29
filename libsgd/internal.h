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

struct HandleTypeInfo {
	ObjectType* const objectType;
	HandleType const handleType;
	HandleTypeInfo(ObjectType* objectType, HandleType handleType);
};

SGD_Handle createHandle(HandleTypeInfo* type, Shared* shared);
SGD_Handle getOrCreateHandle(HandleTypeInfo* type, Shared* shared);
Shared* resolveHandle(HandleTypeInfo* type, SGD_Handle handle);
void destroyHandle(HandleTypeInfo* type, SGD_Handle handle);
bool destroyHandle(HandleTypeInfo* type, Shared* shared);
void destroyHandles(HandleType type);
void destroyAllHandles();

void error(CString message);
void error(CString error, CString message);
void error(CString error, CFileioEx& ex);

template<class T> HandleTypeInfo* handleTypeInfo();

template<class T> SGD_Handle createHandle(T* shared) {
	return createHandle(handleTypeInfo<T>(), shared);
}

template<class T> SGD_Handle getOrCreateHandle(T* shared) {
	return getOrCreateHandle(handleTypeInfo<T>(), shared);
}

template<class T> T* resolveHandle(SGD_Handle handle) {
	return static_cast<T*>(resolveHandle(handleTypeInfo<T>(), handle));
}

template<class T> void destroyHandle(SGD_Handle handle) {
	destroyHandle(handleTypeInfo<T>(), handle);
}

template<class T> bool destroyHandle(T* shared) {
	return destroyHandle(handleTypeInfo<T>(), shared);
}

// ***** Type infos *****

// clang-format off
#define SGD_HANDLE_TYPE_INFO(T, H) \
inline HandleTypeInfo _sgdHandleTypeInfo_##T{T::staticType(), H}; \
template<> inline HandleTypeInfo* handleTypeInfo<T>(){return &_sgdHandleTypeInfo_##T;}
// clang-format on

SGD_HANDLE_TYPE_INFO(Texture, HandleType::texture);
SGD_HANDLE_TYPE_INFO(Material, HandleType::material);
SGD_HANDLE_TYPE_INFO(Mesh, HandleType::mesh);
SGD_HANDLE_TYPE_INFO(Entity, HandleType::entity);
SGD_HANDLE_TYPE_INFO(Camera, HandleType::entity);
SGD_HANDLE_TYPE_INFO(Light, HandleType::entity);
SGD_HANDLE_TYPE_INFO(Model, HandleType::entity);
SGD_HANDLE_TYPE_INFO(Sprite, HandleType::entity);
SGD_HANDLE_TYPE_INFO(Skybox, HandleType::entity);

}
