#pragma once

#include "std.h"

#include <filesystem>

//! @file

namespace sgd {

struct Path;
using CPath = const Path&;

struct Path {

	enum struct Flags {
		valid = 1,
		url = 1,
		filePath = 2,
	};

	Path() = default;
	explicit Path(CString path);
	explicit Path(const char* path);
	explicit Path(std::filesystem::path path);

	bool isFilePath() const;
	std::filesystem::path filePath() const;

	bool isUrl() const;
	String url() const;

	// File functions only...
	bool empty() const;
	bool exists() const;
	bool isFile() const;
	bool isDir() const;

	String name()const;
	String stem()const;
	String ext()const;

	Path parentDir() const;
	bool createFile(bool createParentDirs) const;
	bool createDir(bool createParentDirs) const;

	explicit operator bool() const {
		return !empty();
	}

	String str() const {
		return m_str;
	}

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
