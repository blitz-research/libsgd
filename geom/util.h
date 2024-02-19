#pragma once

#include <core/exports.h>

#include <limits>

#if 0

Refs:

https://realtimecollisiondetection.net/blog/?p=89

#endif

namespace sgd {

template <class T> bool approxOne(T t) {
	return std::abs(t - T(1)) <= std::numeric_limits<T>::epsilon();
}

} // namespace sgd
