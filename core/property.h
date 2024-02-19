#pragma once

#include "object.h"
#include "signal.h" // NOLINT
#include "std.h"

//! @file

namespace sgd {

template <class T> struct Property : public Object {

	Signal<const T&> changed;

	Property() = default;

	explicit Property(const T& value) : m_value(value) {
	}

	Property(const Property& prop) : m_value(prop.value()) {
	}

	Property(Property&& value) noexcept = delete;

	Property& operator=(const T& value) {
		if (value == m_value) return *this;
		m_value = value;
		changed.emit(m_value);
		return *this;
	}

	Property& operator=(const Property& prop) {
		if (prop.m_value == m_value) return *this;
		m_value = prop.m_value;
		changed.emit(m_value);
		return *this;
	}

	Property& operator=(Property&& value) noexcept = delete;

	explicit operator bool() const {
		return bool(m_value);
	}

	const T& operator()() const {
		return m_value;
	}

	friend std::ostream& operator<<(std::ostream& str, const Property& property) {
		return str << property.m_value;
	}

private:
	T m_value{};
};

} // namespace sgd
