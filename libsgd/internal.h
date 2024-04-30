#pragma once

#include <scene/exports.h>
#include <audio/exports.h>

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
inline OverlayPtr g_overlay;
inline DrawListPtr g_drawList;
inline FontPtr g_defaultFont;

enum struct HandleType {
//	object,
	font,
	sound,
	texture,
	material,
	surface,
	mesh,
	entity,
};

struct HandleTypeInfo {
	ObjectType* const objectType;
	HandleType const handleType;
	HandleTypeInfo(ObjectType* objectType, HandleType handleType);
};

[[noreturn]] void error(CString message);
[[noreturn]] void error(CString error, CString message);
[[noreturn]] void error(CString error, CFileioEx& ex);

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
	if(!g_started) error("LibSGD has not been initialized");
}

inline Window* mainWindow(){
	if(g_mainWindow) return g_mainWindow;
	error("Main window has not been created");
}

inline GraphicsContext* mainGC() {
	if(g_mainGC) return g_mainGC;
	error( "Graphics context does not exist");
}

inline Scene* mainScene(){
	if(g_mainScene) return g_mainScene;
	error("Main scene has not been created");
}

inline DrawList* drawList() {
	if(g_drawList) return g_drawList;
	error("Draw list has not been created");
}

// ***** Type infos *****

// clang-format off
#define SGD_HANDLE_TYPE_INFO(T, H) \
inline HandleTypeInfo _sgdHandleTypeInfo_##T{T::staticType(), H}; \
template<> inline HandleTypeInfo* handleTypeInfo<T>(){return &_sgdHandleTypeInfo_##T;}
// clang-format on

SGD_HANDLE_TYPE_INFO(Font, HandleType::font);
SGD_HANDLE_TYPE_INFO(Sound, HandleType::sound);
SGD_HANDLE_TYPE_INFO(Texture, HandleType::texture);
SGD_HANDLE_TYPE_INFO(Material, HandleType::material);
SGD_HANDLE_TYPE_INFO(Surface, HandleType::surface);
SGD_HANDLE_TYPE_INFO(Mesh, HandleType::mesh);
SGD_HANDLE_TYPE_INFO(Entity, HandleType::entity);
SGD_HANDLE_TYPE_INFO(Camera, HandleType::entity);
SGD_HANDLE_TYPE_INFO(Light, HandleType::entity);
SGD_HANDLE_TYPE_INFO(Model, HandleType::entity);
SGD_HANDLE_TYPE_INFO(Sprite, HandleType::entity);
SGD_HANDLE_TYPE_INFO(Skybox, HandleType::entity);

}
