#pragma once

#include "std.h"

#include <filesystem>

//! @file

namespace sgd {

struct Path;
using CPath = const Path&;

struct Path {
	explicit Path(String str);

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

private:
	String m_str;
};

inline bool operator==(CPath x, CPath y) {
	return x.str() == y.str();
}

inline bool operator!=(CPath x, CPath y) {
	return x.str() != y.str();
}

inline bool operator<(CPath x, CPath y) {
	return x.str() < y.str();
}

Path operator/(CPath x, CPath y);
Path operator/(CPath x, CString y);
Path operator/(CString x, CPath y);
Path operator+(CPath x, CString y);

String appDir();

}
