#pragma once

#include "std.h"

#include <cstdio>
#include <sstream>

#define SGD_LOG log(__FILE__, __FUNCTION__, __LINE__)

namespace sgd {

extern thread_local String debugThreadName;

struct Log {
	std::stringstream buf;
	bool moved{};

	Log() = default;
	Log(bool enabled);
	Log(const Log&) = delete;
	Log(Log&& log) noexcept;
	~Log();
};

template <class T> Log&& operator<<(Log&& log, const T& value) {
	if (!log.buf.str().empty()) log.buf << ' ';
	log.buf << value;
	return std::move(log);
}

Log log(CString file, CString func, int line);

void enableLogging(CString path);

inline Log log() {
	return log({},{},~0);
}

} // namespace sgd
