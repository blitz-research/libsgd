#include "log.h"
#include "config.h"
#include "path.h"
#include "stringutil.h"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>

// #ifdef SGD_COMPILER_MSVC
// #pragma warning(disable : 4996)
// #endif

namespace sgd {

thread_local String debugThreadId = (std::ostringstream() << std::setw(6) << std::this_thread::get_id()).str();

namespace {

using Clock = std::chrono::system_clock;
// using Clock = std::chrono::steady_clock;
// using Clock = std::chrono::high_resolution_clock;

String timeStamp(const Clock::time_point& tp) {

	auto dur = tp.time_since_epoch();
	auto durS = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
	auto durMs = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

	std::time_t tS = durS;

	std::ostringstream str;
	str << std::put_time(std::localtime(&tS), "%Y-%m-%d %H:%M:%S.");
	str << std::setw(3) << std::setfill('0') << int(durMs - durS * 1000);

	return str.str();
}

std::mutex g_mutex;

std::ofstream g_logstream;

bool g_stdoutEnabled = true;

auto init = configVarChanged("log.stdoutEnabled").connect(nullptr, [](CString value) { //
	g_stdoutEnabled = truthiness(value);
});

} // namespace

Log::Log(Log&& log) noexcept : buf(std::move(log.buf)) {
	SGD_ASSERT(!log.moved);
	log.moved = true;
}

Log::~Log() {
	if (moved) return;

	auto str = timeStamp(Clock::now()) + " [" + debugThreadId + "] " + buf.str() + "\n";

	{
		std::lock_guard<std::mutex> lock(g_mutex);

		if (!g_logstream.is_open()) {
			static bool opened;

			if (opened) SGD_ERROR("Logging error, log file unexpectedly closed");

			Path path("~/.sgd/log.txt");
			if (!path.createFile(true)) SGD_ERROR("Logging error, failed to create log file");

			g_logstream.open(path.filePath());
			if (!g_logstream.is_open()) SGD_ERROR("Logging error, failed to open log file");

			opened = true;
		}

		g_logstream << str;
		g_logstream.flush();

		if (g_stdoutEnabled) std::cout << str << std::flush;
	}
}

Log log(CString file, CString func, int line) {

	auto tfile = replace(file, "\\", "/");
	if (startsWith(tfile, SGD_CMAKE_SOURCE_DIR "/")) {
		tfile = tfile.substr(std::char_traits<char>::length(SGD_CMAKE_SOURCE_DIR "/"));
	}

	return Log() << tfile << ("(" + std::to_string(line) + ") :");
}

} // namespace sgd
