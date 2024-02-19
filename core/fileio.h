#pragma once

#include "expected.h"
#include "path.h"

//! @file

namespace sgd {

struct FileioEx;
using CFileioEx = const FileioEx&;

struct FileioEx {
	explicit FileioEx(String message) : m_message(std::move(message)) {
	}

	String message() const {
		return m_message;
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

} // namespace sgd
