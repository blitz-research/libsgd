#include "app.h"

#include <GLFW/glfw3.h>

namespace sgd {

namespace {

int g_suspended;

}

void initApp() {
	static bool done;
	if (done) return;
	done = true;

	if (!glfwInit()) {
		log() << "### glfwInit() failed - aborting";
		SGD_ABORT();
	}
}

void suspendApp() {
	if (++g_suspended == 1) {
		log() << "### App suspended";
		appSuspended.emit();
	}
}

void resumeApp() {
	if (--g_suspended == 0) {
		log() << "### App resumed";
		appResumed.emit();
	}
}

Vec2u desktopSize() {
	auto monitor = glfwGetPrimaryMonitor();
	auto mode = glfwGetVideoMode(monitor);
	return Vec2i{mode->width, mode->height};
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
