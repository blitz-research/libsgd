#pragma once

#include "expected.h"
#include "path.h"

//! @file

#define SGD_FILEIOEX(MSG) sgd::FileioEx(sgd::String(MSG), __FILE__, __LINE__)
#define SGD_PATHEX(MSG, PATH) SGD_FILEIOEX(sgd::String(MSG) + " \"" + (PATH).str() + "\"")

namespace sgd {

struct FileioEx;
using CFileioEx = const FileioEx&;

struct FileioEx {

	FileioEx(String message, const char* file, int line);

	CString message() const {
		return m_message;
	}

	friend std::ostream& operator<<(std::ostream& os, CFileioEx ex) {
		return os << ex.m_message;
	}

private:
	String m_message;
};

//! Load a string from a filesystem path.
Expected<String, FileioEx> loadString(CPath path);

//! Write a string to filesystem path.
Expected<bool, FileioEx> saveString(CString string, CPath path);

//! Load byte data from file.
Expected<Data, FileioEx> loadData(CPath path);

//! Save byte data to file.
Expected<bool, FileioEx> saveData(CData data, CPath path);

//! Save byte data to file.
Expected<bool, FileioEx> saveData(const void* data, size_t size, CPath path);

} // namespace sgd
