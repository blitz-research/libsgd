#include "std.h"

#include <thread>

#if SGD_OS_EMSCRIPTEN
#include "emscripten/html5.h"
EM_JS(void, jsalert, (const char* msg_cstr), {
	const msg = UTF8ToString(msg_cstr);
	alert(msg);
});
#elif SGD_OS_WINDOWS
#include <windows.h>
#undef min
#undef max
#endif

#ifdef SGD_ASAN_ENABLED
// The mere presence of this magic bit of code causes the debugger to popup when address sanitizer detects corruption at
// runtime.
extern "C" const char* __asan_default_options() { // NOLINT (unused function)
	return "abort_on_error=1:detect_leaks=0";
}
#endif

namespace {

using Clock = std::chrono::high_resolution_clock;

Clock::time_point g_timerStart = Clock::now();

std::thread::id g_mainThreadId = std::this_thread::get_id();

} // namespace

namespace sgd {

void unreachable() {
	SGD_BREAK();
	std::abort();
}

void resetTimer() {
	g_timerStart = Clock::now();
}

int64_t nanos() {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - g_timerStart).count();
}

int64_t micros() {
	return nanos() / 1000;
}

int64_t millis() {
	return nanos() / 1000000;
}
String toLower(String str) {
	for (int i = 0; i < str.size(); ++i) str[i] = tolower(str[i]);
	return str;
}

String toUpper(String str) {
	for (int i = 0; i < str.size(); ++i) str[i] = tolower(str[i]);
	return str;
}

bool isMainThread() {
	return std::this_thread::get_id() == g_mainThreadId;
}

void alert(CString msg) {
#if SGD_OS_WINDOWS
	MessageBoxA(GetActiveWindow(), msg.c_str(), "Program Alert", MB_OK);
#elif SGD_OS_EMSCRIPTEN
	jsalert(msg.c_str());
#elif SGD_OS_LINUX
	system(("zenity --info --title \"Program Alert\" --text \"" + msg + "\"").c_str());
#endif
}

void openBrowser(CString url) {
#if SGD_OS_WINDOWS
	ShellExecuteA(HWND_DESKTOP, 0, url.c_str(), 0, 0, SW_SHOWNORMAL);
#elif SGD_OS_LINUX
	std::system((String("xdg-open \"") + url + "\"").c_str());
#endif
}

} // namespace sgd
