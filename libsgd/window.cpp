#include "internal.h"

#include <sgd/window.h>

#include <window/exports.h>

namespace {

sgd::Vec2u g_windowSize;

int g_windowEvents;

} // namespace

void SGD_DECL sgd_CreateWindow(int width, int height, SGD_String title, int flags) {
	if(sgd::mainWindow) sgd_Error("Window has already been created");

	sgd::mainWindow = new sgd::Window(sgd::Vec2u(width, height), title, (sgd::WindowFlags)flags);

	sgd::mainWindow->sizeChanged.connect(nullptr, [](sgd::CVec2u size) {
		g_windowSize = size;
		g_windowEvents |= SGD_EVENT_MASK_SIZE_CHANGED;
	});

	sgd::mainWindow->closeClicked.connect(nullptr, [] { //
		g_windowEvents |= SGD_EVENT_MASK_CLOSE_CLICKED;
	});

	g_windowSize = sgd::mainWindow->size();
}

int SGD_DECL sgd_WindowWidth() {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");

	return g_windowSize.x;
}

int SGD_DECL sgd_WindowHeight() {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");

	return g_windowSize.y;
}

int SGD_DECL sgd_PollEvents() {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");

	g_windowEvents = 0;
	sgd::mainWindow->pollEvents();

	return g_windowEvents;
}
