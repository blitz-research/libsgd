#include "log.h"
#include "stringutil.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>

//#ifdef SGD_COMPILER_MSVC
//#pragma warning(disable : 4996)
//#endif

namespace sgd {

thread_local String debugThreadName = (std::ostringstream() << std::this_thread::get_id()).str();

namespace {

using Clock = std::chrono::system_clock;
// using Clock = std::chrono::steady_clock;
// using Clock = std::chrono::high_resolution_clock;

thread_local String suffix;

std::mutex g_mutex;

static String timeStamp(const Clock::time_point& tp) {

	auto dur = tp.time_since_epoch();
	auto durS = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
	auto durMs = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

	std::ostringstream str;
	str << std::put_time(std::localtime(&durS), "%Y-%m-%d %H:%M:%S.");
	str << std::setw(3) << std::setfill('0') << int(durMs - durS * 1000);

	return str.str();
}

Vector<String> g_enabled;

} // namespace

Log::Log(bool enabled) : moved(!enabled) {
}

Log::Log(Log&& log) noexcept : buf(std::move(log.buf)) {
	log.moved = true;
}

Log::~Log() {
	if (moved) return;
	{
		std::lock_guard<std::mutex> lock(g_mutex);
		std::cout << timeStamp(Clock::now()) << " [" << debugThreadName << "] " << buf.str() << suffix << std::endl;
	}
}

Log log(CString file, CString func, int line) {

	suffix={};
	if (file.empty()) return Log();

	auto tfile = replace(file, "\\", "/");
	if (startsWith(tfile, SGD_CMAKE_SOURCE_DIR "/")) {
		tfile = tfile.substr(std::char_traits<char>::length(SGD_CMAKE_SOURCE_DIR "/"));
	}

	bool enabled = false;
	for (auto p : g_enabled) {
		if (tfile.find(p) == String::npos) continue;
		enabled = true;
		break;
	}
	if (!enabled) return Log(false);

	//	suffix = " " + tfile + "(" + std::to_string(line) + ")";
	return Log() << tfile << ("(" + std::to_string(line) + ") :");
//	return Log();// << func;
}

void enableLogging(CString path) {
	g_enabled.push_back(path);
}

} // namespace sgd
