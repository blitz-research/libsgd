#include "internal.h"

#include <thread>

namespace {

void(SGD_DECL* g_errorHandler)(SGD_String error, void* context);

void* g_errorContext;

} // namespace

void SGD_DECL sgd_Startup() {
}

void SGD_DECL sgd_Shutdown() {
	if(sgd::mainWindow) {
		if(sgd::mainGC) {
			if(sgd::mainScene) {
				sgd::mainScene = nullptr;
			}
			sgd::mainGC->wgpuDevice().Destroy();
			sgd::mainGC=nullptr;
		}
		sgd::mainWindow->close();
		sgd::mainWindow=nullptr;
	}
}

void SGD_DECL sgd_SetErrorHandler(void(SGD_DECL* handler)(SGD_String error, void* context), void* context) {
	g_errorHandler = handler;
	g_errorContext = context;
}

void SGD_DECL sgd_Run(void(SGD_DECL* start)()) {

	std::thread(start).detach();

	sgd::beginAppEventLoop();
}

void SGD_DECL sgd_Error(SGD_String error) {
	if (g_errorHandler) {
		g_errorHandler(error, g_errorContext);
	} else {
		sgd::alert(sgd::String("Unhandled SGD error: ") + error);
	}
	std::exit(1);
}

void SGD_DECL sgd_Alert(SGD_String message) {
	sgd::alert(message);
}
