#include "app.h"

#include <GLFW/glfw3.h>

namespace sgd {

namespace {

bool g_inited;
bool g_exited;

int g_suspended;

}

void initApp() {
	if(g_inited) return;
	g_inited = true;

	if (!glfwInit()) SGD_ERROR("glfwInit() failed, aborting");

	appIniting.emit();
}

void exitApp() {
	if(g_exited || !g_inited) return;
	g_exited = true;

	appExiting.emit();

	glfwTerminate();
}

void suspendApp() {
	if (++g_suspended == 1) {
		appSuspending.emit();
		SGD_LOG << "App suspended";
	}
}

void resumeApp() {
	if (--g_suspended == 0) {
		appResuming.emit();
		SGD_LOG << "App resumed";
	}
}

Vec2u desktopSize() {
	auto monitor = glfwGetPrimaryMonitor();
	auto mode = glfwGetVideoMode(monitor);
	return Vec2i{mode->width, mode->height};
}

uint32_t desktopHz() {
	auto monitor = glfwGetPrimaryMonitor();
	auto mode = glfwGetVideoMode(monitor);
	return mode->refreshRate;
}

void pollEvents() {

	// This is to delay glfwWaitEvents by 1 poll, so user can 'see' suspend events.
	static bool wait;

	runOnMainThread(
		[] { //
			beginPollEvents.emit();
			if (g_suspended && wait) {
				glfwWaitEvents();
			} else {
				glfwPollEvents();
			}
			wait = g_suspended;
			endPollEvents.emit();
		},
		true);
}

} // namespace sgd
