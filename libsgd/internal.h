#pragma once

#include <graphics/exports.h>
#include <window/exports.h>

#include <sgd/sgd.h>

namespace sgdx {

using namespace sgd;

struct SGD_Event {
	int type;
};

inline bool g_started;
inline WindowPtr g_mainWindow;
inline GraphicsContextPtr g_mainGC;
inline ScenePtr g_mainScene;

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

void error(CString message);
void error(CString error, CString message);
void error(CString error, CFileioEx& ex);

void postEvent(SGD_Event event);
void getEventQueue(Deque<SGD_Event>& queue);

SGD_Handle createHandle(HandleTypeInfo* type, Shared* shared);
SGD_Handle getOrCreateHandle(HandleTypeInfo* type, Shared* shared);
Shared* resolveHandle(HandleTypeInfo* type, SGD_Handle handle);
void destroyHandle(HandleTypeInfo* type, SGD_Handle handle);
bool destroyHandle(HandleTypeInfo* type, Shared* shared);
void destroyHandles(HandleType type);
void destroyAllHandles();

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

inline void started() {
	if(!g_started) error("Startup has not been called");
}

inline Window* mainWindow(){
	if(!g_mainWindow) error("Main window does not exist");
	return g_mainWindow;
}

inline GraphicsContext* mainGC() {
	if(!g_mainGC) error( "Graphics context does not exist");
	return g_mainGC;
}

inline Scene* mainScene(){
	if(!g_mainScene) error("Main scene does not exist");
	return g_mainScene;
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
