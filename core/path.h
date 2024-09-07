#pragma once

#include "std.h"

#include <filesystem>

//! @file

namespace sgd {

struct Path;
using CPath = const Path&;

struct Path {

	Path() = default;
	explicit Path(CString path);
	explicit Path(const char* path);
	explicit Path(const std::filesystem::path& path);

	bool isFilePath() const;
	std::filesystem::path filePath() const;

	bool isUrl() const;
	String url() const;

	bool empty() const;

	explicit operator bool() const {
		return !empty();
	}

	String str() const {
		return m_str;
	}

	// File or URL...
	Path parentPath() const;
	String name()const;
	String stem()const;
	String ext()const;

	// File only...
	Path absolutePath() const;
	bool exists() const;
	bool isFile() const;
	bool isDir() const;

	bool createFile(bool createParents) const;
	bool createDir(bool createParents) const;

private:
	String m_str;
	String m_url;

	void init(CString str);
};

bool operator==(CPath x, CPath y);
bool operator!=(CPath x, CPath y);
bool operator<(CPath x, CPath y);

Path operator/(CPath x, CPath y);
Path operator/(CPath x, CString y);
Path operator/(CString x, CPath y);
Path operator+(CPath x, CString y);

std::ostream& operator<<(std::ostream& os, CPath path);

} // namespace sgd
