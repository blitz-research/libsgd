#pragma once

#include "std.h"

#include <condition_variable>
#include <mutex>

namespace sgd {
template <class T> struct CondVar {

	void set(const T& value);

	const T& get() const {
		return m_value;
	}

	void wait(CFunction<bool(const T&)> condition) const;

	void waiteq(const T& value) const;

	void waitne(const T& value) const;

private:
	T m_value{};
	mutable std::mutex m_mutex;
	mutable std::condition_variable m_cv;
};

template <class T> void CondVar<T>::set(const T& value) {
	if (value == m_value) return;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_value = value;
	}
	m_cv.notify_all();
}

template <class T> void CondVar<T>::wait(CFunction<bool(const T&)> cond) const {
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cv.wait(lock, cond(m_value));
	}
}

template <class T> void CondVar<T>::waiteq(const T& value) const {
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cv.wait(lock, [=] { return m_value == value; });
	}
}

template <class T> void CondVar<T>::waitne(const T& value) const {
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cv.wait(lock, [=] { return m_value != value; });
	}
}

} // namespace sgd
