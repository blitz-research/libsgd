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

	sgd::mainWindow->closeClicked.connect(sgd::mainWindow, [] { //
		g_windowEvents |= SGD_WINDOW_EVENT_MASK_CLOSE_CLICKED;
	});

	sgd::mainWindow->sizeChanged.connect(sgd::mainWindow, [](sgd::CVec2u size) {
		g_windowSize = size;
		g_windowEvents |= SGD_WINDOW_EVENT_MASK_SIZE_CHANGED;
		sgd::log() << "### sizeChanged"<<size;
	});
	g_windowSize = sgd::mainWindow->size();
}

SGD_API void sgd_Run(void (*main)()) {
	SGD_ASSERT(sgd::isMainThread() && sgd::mainWindow);

	std::thread(main).detach();

	sgd::beginAppEventLoop();
}

SGD_API int sgd_WindowWidth() {
	SGD_ASSERT(!sgd::isMainThread() && sgd::mainWindow);

	return g_windowSize.load().x;
}

SGD_API int sgd_WindowHeight() {
	SGD_ASSERT(!sgd::isMainThread() && sgd::mainWindow);

	return g_windowSize.load().y;
}

SGD_API int sgd_PollEvents() {
	SGD_ASSERT(!sgd::isMainThread() && sgd::mainWindow);

	g_windowEvents=0;

	sgd::runOnMainThread(
		[] { //
			sgd::mainWindow->pollEvents();
		},
		true);

	return g_windowEvents;
}

SGD_API void sgd_Render() {
	SGD_ASSERT(!sgd::isMainThread() && sgd::mainWindow);

	sgd::runOnMainThread([] { sgd::mainWindow->render(); }, true);
}
