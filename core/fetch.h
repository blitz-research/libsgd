#pragma once

#include "std.h"
#include "expected.h"

namespace sgd {

struct FetchEx;
using CFetchEx = const FetchEx&;

struct FetchEx {
	explicit FetchEx(String message) : m_message(std::move(message)) {
	}

	CString message() const {
		return m_message;
	}

private:
	String m_message;
};

Expected<String, FetchEx> fetchString(CString url);

Expected<Data, FetchEx>  fetchData(CString url);

}
