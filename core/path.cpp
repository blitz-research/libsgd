#include "path.h"

#include "log.h"
#include "stringutil.h"

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

} // namespace

Path::Path(String path) : m_str(std::move(path)) {
}

Path::Path(std::filesystem::path& fpath) : m_str(fpath.u8string()) {
}

bool Path::isUrl() const {
	return m_str.find("://") != String::npos;
}

bool Path::isValidFilePath() const {
	return !isUrl();
}

std::filesystem::path Path::filePath() const {
	if (startsWith(m_str, "${")) {
		auto i = m_str.find('}', 2);
		if (i == String::npos) return {};

		auto id = m_str.substr(2, i - 2);
		++i;

		std::filesystem::path prefix;

		if (id == "HOME") {
#if SGD_OS_WINDOWS
			auto home = getenv("USERPROFILE");
			if (!home) home = getenv("HOMEPATH");
#else
			auto home = getenv("HOME");
#endif
			if (!home) return {};
			prefix = home;
		}else{
			return {};
		}
		while(i < m_str.size() && m_str[i] == '/' || m_str[i] == '\\') ++i;
		return prefix / m_str.substr(i);
	}
	return std::filesystem::absolute(m_str).u8string();
}

bool Path::exists() const {
	return std::filesystem::exists(filePath());
}

bool Path::isFile() const {
	return std::filesystem::is_regular_file(filePath());
}

bool Path::isDir() const {
	return std::filesystem::is_directory(filePath());
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

Path appPath() {
	static Path path;
	if (path) return path;

#if SGD_OS_WINDOWS

	char buf[MAX_PATH + 1];
	GetModuleFileName(GetModuleHandle(nullptr), buf, MAX_PATH);
	buf[MAX_PATH] = 0;

	return path = Path(replace(buf, "\\", "/"));

#elif defined(SGD_OS_LINUX) || defined(SGD_OS_MACOS)

	char lnk[PATH_MAX + 1];
	char buf[PATH_MAX + 1];
	pid_t pid = getpid();
	sprintf(lnk, "/proc/%i/exe", pid);
	int i = readlink(lnk, buf, PATH_MAX);
	if (i > PATH_MAX) SGD_ABORT();
	buf[i] = 0;

	return path = Path(buf);

#elif SGD_OS_EMSCRIPTEN

	return path = Path("/");

#else
	SGD_ABORT();
#endif
}

Path homeDir() {
#if SGD_OS_WINDOWS
	auto home = getenv("USERPROFILE");
	SGD_ASSERT(home);
	return Path(replace(home, "\\", "/"));
#else
	auto home = getenv("HOME");
	SGD_ASSERT(home);
	return Path(home);
#endif
}

} // namespace sgd
