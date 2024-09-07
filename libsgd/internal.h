#pragma once

#include <audio/exports.h>
#include <scene/exports.h>

#include <sgd/sgd.h>

namespace sgdx {

using namespace sgd;

struct SGD_Event {
	int type;
};

inline bool g_started;
inline bool g_terminated;
inline WindowPtr g_mainWindow;
inline ScenePtr g_mainScene;
inline OverlayPtr g_overlay;
inline DrawListPtr g_drawList;

[[noreturn]] void error(CString message);
[[noreturn]] void error(CString error, CString message);
[[noreturn]] void error(CString error, CFileioEx& ex);

inline void started() {
	if (!g_started) error("LibSGD has not been initialized");
}

inline Window* mainWindow() {
	if (g_mainWindow) return g_mainWindow;
	error("Main window has not been created");
}

inline Scene* mainScene() {
	if (g_mainScene) return g_mainScene;
	error("Main scene has not been created");
}

inline DrawList* drawList() {
	if (g_drawList) return g_drawList;
	error("Draw list has not been created");
}

void postEvent(SGD_Event event);
void getEventQueue(Deque<SGD_Event>& queue);

SGD_Handle createHandle(Shared* shared);
SGD_Handle getHandle(Shared* shared);
SGD_Handle getOrCreateHandle(Shared* shared);
Shared* resolveHandle(SGD_Handle handle);
void releaseHandle(SGD_Handle handle);
void releaseHandle(Shared* shared);
void releaseAllHandles();
void releaseAllHandles(ObjectType* ofType);

template<class T> T* resolveHandle(SGD_Handle handle) {
	auto shared = resolveHandle(handle);
	return shared->as<T>();
}

} // namespace sgdx
