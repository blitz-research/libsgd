#pragma once

#include "std.h"

#include <condition_variable>
#include <mutex>

namespace sgd {

template <class T> struct CondVar {

	CondVar& operator=(const T& value ){
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (value == m_value) return *this;
			m_value = value;
		}
		m_cv.notify_all();
		return *this;
	}

	const T& operator()()const {
		return m_value;
	}

	template<class F> void wait(F pred) const {
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cv.wait(lock, pred);
		}
	}

	void waiteq(const T& value) const {
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cv.wait(lock, [=] { return m_value == value; });
		}
	}

	void waitne(const T& value) const {
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cv.wait(lock, [=] { return m_value != value; });
		}
	}

private:
	T m_value{};
	mutable std::mutex m_mutex;
	mutable std::condition_variable m_cv;
};

} // namespace sgd
