#pragma once

#include "std.h"

#include <filesystem>

//! @file

namespace sgd {

struct Path;
using CPath = const Path&;

struct Path {
	Path() = default;
	explicit Path(String str);
	explicit Path(std::filesystem::path& fpath);

	bool isUrl() const;

	bool isValidFilePath() const;
	std::filesystem::path filePath() const;

	bool exists() const;
	bool isFile() const;
	bool isDir() const;

	String name() const;
	String stem() const;
	String ext() const;

	String str() const {
		return m_str;
	}

	explicit operator bool() const { return !m_str.empty();}

private:
	String m_str;
};

bool operator==(CPath x, CPath y);
bool operator!=(CPath x, CPath y);
bool operator<(CPath x, CPath y);

Path operator/(CPath x, CPath y);
Path operator/(CPath x, CString y);
Path operator/(CString x, CPath y);
Path operator+(CPath x, CString y);

Path appPath();

inline std::ostream& operator << (std::ostream& os, CPath path) {
	return os << path.str();
}

inline bool operator==(CPath x, CPath y) {
	return x.str() == y.str();
}

inline bool operator!=(CPath x, CPath y) {
	return x.str() != y.str();
}

inline bool operator<(CPath x, CPath y) {
	return x.str() < y.str();
}
}
