#pragma once

#include "shared.h"

#define SGD_SHARED(T)                                                                                                          \
	struct T;                                                                                                                  \
	using C##T = const T;                                                                                                      \
	using T##Ptr = SharedPtr<T>;                                                                                               \
	using C##T##Ptr = SharedPtr<const T>;

//! @file

namespace sgd {

template <class T> struct SharedPtr;
template <class T> using CSharedPtr = SharedPtr<const T>;

template <class T> struct SharedPtr {

	SharedPtr() = default;

	SharedPtr(T* ptr) : m_ptr(ptr) {
		retain();
	}

	SharedPtr(const SharedPtr& shared) : m_ptr(shared.m_ptr) {
		retain();
	}

	SharedPtr(SharedPtr&& shared) noexcept : m_ptr(shared.m_ptr) {
		shared.m_ptr = nullptr;
	}

	template <class D> SharedPtr(const SharedPtr<D>& shared) : m_ptr(shared.m_ptr) {
		retain();
	}

	template <class D> SharedPtr(SharedPtr<D>&& shared) noexcept : m_ptr(shared.m_ptr) {
		shared.m_ptr = nullptr;
	}

	~SharedPtr() {
		release();
	}

	SharedPtr& operator=(T* ptr) {
		if (m_ptr == ptr) return *this;
		release();
		m_ptr = ptr;
		retain();
		return *this;
	}

	SharedPtr& operator=(const SharedPtr& shared) {
		if (m_ptr == shared.m_ptr) return *this;
		shared.retain();
		release();
		m_ptr = shared.m_ptr;
		return *this;
	}

	SharedPtr& operator=(SharedPtr&& shared) noexcept {
		if (m_ptr == shared.m_ptr) return *this;
		release();
		m_ptr = shared.m_ptr;
		shared.m_ptr = nullptr;
		return *this;
	}

	template <class D> SharedPtr& operator=(const SharedPtr<D>& shared) {
		if (m_ptr == shared.m_ptr) return *this;
		shared.retain();
		release();
		m_ptr = shared.m_ptr;
		return *this;
	}

	template <class D> SharedPtr& operator=(SharedPtr<D>&& shared) noexcept {
		if (m_ptr == shared.m_ptr) return *this;
		release();
		m_ptr = shared.m_ptr;
		shared.m_ptr = nullptr;
		return *this;
	}

	T* operator->() const {
#if SGD_DEBUG_SHARED_REFS
		((Shared*)m_ptr)->validateRefs();
#endif
		return m_ptr;
	}

	T& operator*() const {
#if SGD_DEBUG_SHARED_REFS
		((Shared*)m_ptr)->validateRefs();
#endif
		return *m_ptr;
	}

	operator T*() const& {
#if SGD_DEBUG_SHARED_REFS
		if (m_ptr) ((Shared*)m_ptr)->validateRefs();
#endif
		return m_ptr;
	}

	T* reset() {
		auto p = m_ptr;
		m_ptr = nullptr;
		return p;
	}

	T* get() const {
		return m_ptr;
	}

private:
	template <class> friend class SharedPtr;

	T* m_ptr{};

	void retain() const {
		if (m_ptr) ((Shared*)m_ptr)->retain();
	}

	void release() const {
		if (m_ptr) ((Shared*)m_ptr)->release();
	}
};

template <class T> SharedPtr<T> shared(T* ptr) {
	return ptr;
}

template <class T> CSharedPtr<T> shared(const T* ptr) {
	return ptr;
}

} // namespace sgd
