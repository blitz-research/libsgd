#include "appevent.h"

#include "log.h"

#include <condition_variable>
#include <deque>
#include <mutex>

#if SGD_OS_EMSCRIPTEN
#include "emscripten/threading.h"
#endif

// Note: Must use unique_lock for condvars

namespace sgd {

namespace {

struct Die {};

std::deque<AppEventFunc> g_deque;
std::mutex g_dequeMutex;

size_t g_enqueued;
std::condition_variable g_enqueuedCV;

size_t g_dispatched;
std::condition_variable g_dispatchedCV;

volatile bool g_running;

void waitEvent() {
	AppEventFunc func;
	{
		std::unique_lock<std::mutex> lock(g_dequeMutex);
		if(!g_running) return;
		//
		g_enqueuedCV.wait(lock, [] { return !g_deque.empty(); });
		func = g_deque.front();
	}
	func();
	{
		std::lock_guard<std::mutex> lock(g_dequeMutex);
		if(!g_running) return;
		//
		g_deque.pop_front();
		++g_dispatched;
	}
	g_dispatchedCV.notify_all();
}

} // namespace

void runOnMainThread(AppEventFunc func, bool sync) {
	if (sync && isMainThread()) {
		func();
		return;
	}
	size_t enqueued;
	{
		std::lock_guard<std::mutex> lock(g_dequeMutex);
		if(!g_running) return;
		//
		g_deque.emplace_back(std::move(func));
		enqueued = ++g_enqueued;
	}
	g_enqueuedCV.notify_one();

#if SGD_OS_EMSCRIPTEN
	emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_V, &waitEvent);
#endif

	if (sync) {
		std::unique_lock<std::mutex> lock(g_dequeMutex);
		if(!g_running) return;
		//
		g_dispatchedCV.wait(lock, [=] { return g_dispatched >= enqueued; });
	}
}

void beginAppEventLoop() {
	SGD_ASSERT(!g_running);
	g_running = true;

#if SGD_OS_EMSCRIPTEN

	emscripten_exit_with_live_runtime();

	SGD_ABORT();

#else

	std::atexit([] {
		g_running = false;
	});

	while (g_running) {
		waitEvent();
	}

#endif
}

void endAppEventLoop() {
	runOnMainThread([] { g_running = false; }, true);
}

} // namespace sgd
