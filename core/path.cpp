#include "path.h"

#include "log.h"
#include "stringutil.h"

#include <fstream>
#include <sstream>

#include <iostream>

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

CString homeDir() {
	static String dir;
	if(!dir.empty()) return dir;
#if SGD_OS_WINDOWS
	auto home = getenv("USERPROFILE");
	if (!home) home = getenv("HOMEPATH");
#else
	auto home = getenv("HOME");
#endif
	SGD_ASSERT(home);
	return dir = home;
}

String join(CString x, CString y) {
	auto i0 = x.find_last_not_of('/');
	auto i1 = y.find_first_not_of('/');
	return (i0 != String::npos ? x.substr(0, i0 + 1) : x) + "/" + (i1 != String::npos ? y.substr(i1) : y);
}

} // namespace

void Path::init(CString str) {
	// TODO: Better file/url validity checking
	m_str = replace(str, "\\", "/");
	if (m_str.find("://") != String::npos) {
		m_url = m_str;
	}
}

Path::Path(CString path) {
	init(path);
}

Path::Path(const char* path) {
	init(path);
}

Path::Path(const std::filesystem::path& path) : m_str(path.u8string()) {
}

bool Path::isFilePath() const {
	return !m_str.empty() && m_url.empty();
}

std::filesystem::path Path::filePath() const {
	SGD_ASSERT(isFilePath());
	if (!m_str.empty() && m_str.front() == '~') return homeDir() + m_str.substr(1);
	return m_str;
}

bool Path::isUrl() const {
	return !m_url.empty();
}

String Path::url() const {
	SGD_ASSERT(isUrl());
	return m_url;
}

bool Path::empty() const {
	return m_str.empty();
}

Path Path::parentPath() const {
	if (isFilePath()) return Path(filePath().parent_path());

	// Find last '/' and remove it unless it's end of '//'.
	auto i = m_url.find_last_of('/');
	if (i == String::npos) return *this;

	// Don't return empty string or strip trailing '//'
	if (i==0 || m_url[i - 1] == '/') return Path(m_url.substr(0, i+1));

	// Safe to remove trailing '/'
	return Path(m_url.substr(0, i));
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

Path Path::absolutePath() const {
	return Path(std::filesystem::absolute(filePath()));
}

bool Path::exists() const {
	return std::filesystem::exists(filePath());
}

bool Path::isFile() const {
	return std::filesystem::is_regular_file(filePath());
}

bool Path::isDir() const {
	SGD_ASSERT(isFilePath());
	return std::filesystem::is_directory(filePath());
}

bool Path::createFile(bool createParentDirs) const {
	if (createParentDirs) {
		auto pdir = parentPath();
		if (pdir && !pdir.createDir(true)) return false;
	}
	std::ofstream fs(filePath(), std::ios::binary | std::ios::trunc);
	bool ok = fs.good();
	fs.close();
	return ok;
}

bool Path::createDir(bool createParentDirs) const { // NOLINT (Recursive)
	if (isDir()) return true;
	if (createParentDirs) {
		auto pdir = parentPath();
		if (pdir && !pdir.createDir(true)) return false;
	}
	return std::filesystem::create_directory(filePath());
}


bool operator==(CPath x, CPath y) {
	return x.str() == y.str();
}

bool operator!=(CPath x, CPath y) {
	return x.str() != y.str();
}

bool operator<(CPath x, CPath y) {
	return x.str() < y.str();
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

std::ostream& operator<<(std::ostream& os, CPath path) {
	return os << path.str();
}

} // namespace sgd
