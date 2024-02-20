#include "window.h"

#include "internal.h"

#include <window/exports.h>

#include <thread>

namespace {

volatile std::atomic<sgd::Vec2u> g_windowSize;

volatile std::atomic_int g_windowEvents;

} // namespace

SGD_API void sgd_CreateWindow(int width, int height, SGD_String title, int flags) {
	SGD_ASSERT(!sgd::mainWindow);

	sgd::mainWindow = new sgd::Window(sgd::Vec2u(width, height), title, (sgd::WindowFlags)flags);

	sgd::mainWindow->sizeChanged.connect(nullptr, [](sgd::CVec2u size) {
		g_windowSize = size;
		g_windowEvents |= SGD_WINDOW_EVENT_MASK_SIZE_CHANGED;
	});

	sgd::mainWindow->closeClicked.connect(nullptr, [] { //
		g_windowEvents |= SGD_WINDOW_EVENT_MASK_CLOSE_CLICKED;
	});

	g_windowSize = sgd::mainWindow->size();
}

SGD_API int sgd_WindowWidth() {
	SGD_ASSERT(sgd::mainWindow);

	return g_windowSize.load().x;
}

SGD_API int sgd_WindowHeight() {
	SGD_ASSERT(sgd::mainWindow);

	return g_windowSize.load().y;
}

SGD_API int sgd_PollEvents() {
	SGD_ASSERT(sgd::mainWindow);

	g_windowEvents=0;

	sgd::mainWindow->pollEvents();

	return g_windowEvents;
}
