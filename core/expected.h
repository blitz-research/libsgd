#pragma once

#include "std.h"

//! @file

namespace sgd {

template <class T, class E> struct Expected {

	Expected(const Expected& ex) : m_isResult(ex.m_isResult) {
		if (m_isResult) new (&m_result) T(ex.m_result);
		if (!m_isResult) new (&m_error) E(ex.m_error);
	}
	Expected(Expected&& ex) : m_isResult(ex.m_isResult) {
		if (m_isResult) new (&m_result) T(std::move(ex.m_result));
		if (!m_isResult) new (&m_error) E(std::move(ex.m_error));
	}

	Expected(const T& result) : m_result(result), m_isResult(true) {
	}
	Expected(T&& result) : m_result(std::move(result)), m_isResult(true) {
	}

	Expected(const E& error) : m_error(error), m_isResult(false) {
	}
	Expected(E&& error) : m_error(std::move(error)), m_isResult(false) {
	}

	~Expected() {
		if (m_isResult) m_result.~T();
		if (!m_isResult) m_error.~E();
	}

	bool isResult() const {
		return m_isResult;
	}

	operator bool() const {
		return m_isResult;
	}

	const T& result() const {
		SGD_ASSERT(m_isResult);
		return m_result;
	}

	const E& error() const {
		SGD_ASSERT(!m_isResult);
		return m_error;
	}

private:
	union {
		T m_result;
		E m_error;
	};
	bool m_isResult;
};

} // namespace sgd
