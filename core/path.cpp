#include "path.h"

#include "log.h"

#include <fstream>
#include <sstream>

#if SGD_OS_WINDOWS
#include <Windows.h>
#undef min
#undef max
#elif SGD_OS_LINUX
#include <linux/limits.h>
#include <unistd.h>
#elif SGD_OS_MACOS
#include <sys/types.h>
#include <unistd.h>
#endif

namespace sgd {

namespace {

String join(CString x, CString y) {
	auto i0 = x.find_last_not_of('/');
	auto i1 = y.find_first_not_of('/');
	return (i0 != String::npos ? x.substr(0, i0 + 1) : x) + "/" + (i1 != String::npos ? y.substr(i1) : y);
}

std::filesystem::path appPath() {
	static std::filesystem::path path;
	if(!path.empty()) return path;

#if SGD_OS_WINDOWS

	wchar_t buf[MAX_PATH + 1];
	GetModuleFileNameW(GetModuleHandleW(0), buf, MAX_PATH);
	buf[MAX_PATH] = 0;
	for (int i = 0; buf[i]; ++i) {
		if (buf[i] == '\\') buf[i] = '/';
	}
	return path = buf;

#elif defined(SGD_OS_LINUX) || defined(SGD_OS_MACOS)

	char lnk[PATH_MAX + 1];
	char buf[PATH_MAX + 1];
	pid_t pid = getpid();
	sprintf(lnk, "/proc/%i/exe", pid);
	int i = readlink(lnk, buf, PATH_MAX);
	if (i > PATH_MAX) SGD_ABORT();
	buf[i] = 0;
	return path = buf;

#elif SGD_OS_EMSCRIPTEN

	return path = "/";

#else
	SGD_ABORT();
#endif
}

} // namespace

Path::Path(String path) : m_str(std::move(path)) {
}

std::filesystem::path Path::resolve() const {

	// Check . dir...
	auto path = std::filesystem::current_path() / m_str;
	if(std::filesystem::exists(path)) return path;

	// Check ./assets dir...
	path = std::filesystem::current_path() / "assets" / m_str;
	if(std::filesystem::exists(path)) return path;

	// Check exe/assets dir...
	path = appPath().parent_path() / m_str;
	if(std::filesystem::exists(path)) return path;

	return {};
}

bool Path::exists() const {
	return std::filesystem::exists(resolve());
}

bool Path::isFile() const {
	return std::filesystem::is_regular_file(resolve());
}

bool Path::isDir() const {
	return std::filesystem::is_directory(resolve());
}

String Path::name() const {
	auto i = m_str.find_last_of('/');
	return i != String::npos ? m_str.substr(i + 1) : m_str;
}

String Path::stem() const {
	auto t = name();
	auto i = t.find_last_of('.');
	return i == String::npos ? t : t.substr(0, i);
}

String Path::ext() const {
	auto t = name();
	auto i = t.find_last_of('.');
	return i == String::npos ? String{} : t.substr(i);
}

Path operator/(CPath x, CPath y) {
	return Path(join(x.str(), y.str()));
}

Path operator/(CPath x, CString y) {
	return Path(join(x.str(), y));
}

Path operator/(CString x, CPath y) {
	return Path(join(x, y.str()));
}

Path operator+(CPath x, CString y) {
	return Path(x.str() + y);
}

} // namespace sgd
