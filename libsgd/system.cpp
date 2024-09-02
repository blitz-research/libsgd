#include "internal.h"

#include <thread>

#if SGD_OS_WINDOWS
#include <clocale>
#endif

namespace {

sgd::Deque<sgdx::SGD_Event> g_eventQueue;

} // namespace

void SGD_DECL sgd_Init() {
	if (sgdx::g_started) return;
	sgdx::g_started = true;

#if SGD_OS_WINDOWS
	setlocale(LC_ALL, ".utf8");
#endif

	sgd::initApp();

	sgd::appSuspending.connect(nullptr, [] { sgdx::postEvent({SGD_EVENT_MASK_SUSPENDED}); });

	sgd::appResuming.connect(nullptr, [] { sgdx::postEvent({SGD_EVENT_MASK_RESUMED}); });
}

void SGD_DECL sgd_Terminate() {
	if (sgdx::g_terminated || !sgdx::g_started) return;
	sgdx::g_terminated = true;

	if(sgdx::g_mainWindow) {
		if(sgdx::g_mainScene) {
			sgdx::g_mainScene=nullptr;
			sgdx::g_overlay=nullptr;
			sgdx::g_drawList=nullptr;
			sgd::destroyGC();
		}
		sgdx::g_mainWindow->close();
		sgdx::g_mainWindow = nullptr;
	}
	sgd::exitApp();
}

void SGD_DECL sgd_SetConfigVar(SGD_String name, SGD_String value) {
	sgd::setConfigVar(name, value);
}

SGD_String SGD_DECL sgd_GetConfigVar(SGD_String name) {
	static sgd::String result;
	result = sgd::getConfigVar(name) + '\0';
	return result.data();
}

void SGD_DECL sgd_SetErrorHandler(void(SGD_DECL* handler)(SGD_String error, void* context), void* context) {
	sgd::setErrorHandler([=](sgd::CString msg, const char *file, int line){
		handler(msg.c_str(), context);
	});
}

void SGD_DECL sgd_Error(SGD_String error) {
	SGD_ERROR(error);
}

void SGD_DECL sgd_Alert(SGD_String message) {
	sgd::alert(message);
}

void SGD_DECL sgd_Log(SGD_String line) {
	sgd::log() << line;
}

int SGD_DECL sgd_GetDesktopWidth() {
	return (int)sgd::desktopSize().x;
}

int SGD_DECL sgd_GetDesktopHeight() {
	return (int)sgd::desktopSize().y;
}

int SGD_DECL sgd_PollEvents() {
	sgdx::started();

	sgd::pollEvents();
	g_eventQueue.clear();
	getEventQueue(g_eventQueue);
	int mask = 0;
	for (auto& ev : g_eventQueue) mask |= ev.type;
	return mask;
}

void SGD_DECL sgd_ReleaseHandle(SGD_Handle handle) {
	sgdx::releaseHandle(handle);
}

void SGD_DECL sgd_ReleaseAllHandles() {
	sgdx::releaseAllHandles();
}

void SGD_DECL sgd_DebugMemory() {
	sgdx::started();

	static sgdx::Map<sgdx::ObjectType*, int> counts;
	static const sgdx::String pad = "                   ";

	for (auto type = sgd::ObjectType::allTypes(); type; type = type->succ) {
		int count = type->instanceCount();
		int diff = count - counts[type];
		counts[type] = count;
		if (!diff) continue; // count && !diff) continue;
		sgd::String dstr = diff ? (sgd::String(diff > 0 ? "(+" : "(") + std::to_string(diff) + ')') : "";
		sgd::String name = (sgd::String(type->name) + pad).substr(0, pad.size());
		sgd::log() << name << count << dstr;
	}
	sgdx::log() << "---------------------------------";
}
