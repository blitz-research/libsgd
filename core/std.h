#pragma once

#include <algorithm>
#include <any>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

// clang-format off
#if SGD_COMPILER_MSVC
#define SGD_BREAK() {__debugbreak();std::abort();}
#elif SGD_OS_EMSCRIPTEN
#define SGD_BREAK() {__builtin_trap();std::abort();}
//#define SGD_BREAK() emscripten_force_exit(-1)
#else
#define SGD_BREAK() {__builtin_trap();std::abort();}
#endif

#if SGD_CONFIG_RELEASE
#define SGD_PANIC(MSG) {sgd::alert(MSG);std::abort();}
#define SGD_ABORT() {SGD_PANIC("Internal SGD Error");}
#define SGD_ASSERT(COND)
#else
#define SGD_PANIC(MSG) {sgd::alert(MSG);SGD_BREAK()}
#define SGD_ABORT() {SGD_PANIC("Internal SGD Error");}
#define SGD_ASSERT(COND) if (!(COND)) {SGD_PANIC("SGD_ASSERT failed: " #COND);}
#endif
// clang-format on

#if SGD_OS_EMSCRIPTEN
#include <emscripten.h>
#define EM_CALLBACK extern "C" EMSCRIPTEN_KEEPALIVE
#define EM_EXTERN extern "C"
#endif

//! @file

//! Namespace sgd
namespace sgd {

#if SGD_ARCH_64_BIT
using real = double;
#else
using real = float;
#endif

using Any = std::any;
using CAny = const Any&;

using String = std::string;
using CString = const String&;

template <class T> using UniquePtr = std::unique_ptr<T>;
template <class T> using CUniquePtr = const UniquePtr<T>&;

template <class F> using Function = std::function<F>;
template <class F> using CFunction = const Function<F>&;

template <class T> using Vector = std::vector<T>;
template <class T> using CVector = const Vector<T>&;

template <class T> using Deque = std::deque<T>;
template <class T> using CDeque = const Deque<T>&;

template <class T, size_t N> using Array = std::array<T, N>;
template <class T, size_t N> using CArray = const Array<T, N>&;

template <class T, class C = std::less<T>> using Set = std::set<T, C>;
template <class T, class C = std::less<T>> using CSet = const Set<T, C>&;

template <class K, class V, class C = std::less<K>> using Map = std::map<K, V, C>;
template <class K, class V, class C = std::less<K>> using CMap = const Map<K, V, C>&;

template <class K, class V> using Pair = std::pair<K, V>;
template <class K, class V> using CPair = const Pair<K, V>&;

using Data = Vector<uint8_t>;
using CData = const Data&;

[[noreturn]] void unreachable();

// Same as mod but handles negative x value so there's no 'hole' around 0, y should still be positive.
inline int floorMod(int x, int y) {
	return x >= 0 ? x % y : x - ((x - y + 1) / y) * y;
}

// Same as std::fmod but handles negative x value so there's no 'hole' around 0, y should still be positive.
inline float floorMod(float x, float y) {
	return x - std::floor(float(x) / float(y)) * y;
}

template <class C, class V> bool contains(const C& container, const V& value) {
	return std::find(container.begin(), container.end(), value) != container.end();
}

template <class C, class V> bool remove(C& container, const V& value) {
	auto it = std::find(container.begin(), container.end(), value);
	if (it == container.end()) return false;
	container.erase(it);
	return true;
}

template <class C, class V> bool rremove(C& container, const V& value) {
	auto it = std::find(container.rbegin(), container.rend(), value);
	if (it == container.rend()) return false;
	container.erase(--it.base()); // Woah...
	return true;
}

template <class C, class V> typename C::const_iterator find(const C& container, const V& value) {
	return std::find(container.begin(), container.end(), value);
}

template <class C, class V> typename C::iterator find(C& container, const V& value) {
	return std::find(container.begin(), container.end(), value);
}

template <class T> String toString(const T& value) {
	return (std::stringstream() << value).str();
}

template <class T> bool is(CAny value) {
	return std::any_cast<T>(&value) != nullptr;
}

template <class T> T as(CAny value) {
	return std::any_cast<T>(value);
}

template <class T> const T* as(const Any* value) {
	return std::any_cast<T>(value);
}

template <class T> T* as(Any* value) {
	return std::any_cast<T>(value);
}

template <class T> void clear(T* dst, size_t count) {
	std::memset(dst, 0, count * sizeof(T));
}

template <class T> void copy(T* dst, const T* src, size_t count) {
	std::memcpy(dst, src, count * sizeof(T));
}

template <class T> void fill(T* dst, const T& val, size_t count) {
	while (count--) *dst++ = val;
}

int64_t millis();
int64_t micros();
int64_t nanos();

String toUpper(String str);
String toLower(String str);

bool isMainThread();

void alert(CString msg);

void openBrowser(CString url);

// ***** Math *****

constexpr float pi = 3.14159265359f;
constexpr float twoPi = pi * 2.0f;
constexpr float halfPi = pi * .5f;
constexpr float degreesToRadians = .01745329252f;
constexpr float radiansToDegrees = 57.295779513f;

inline uint8_t ftou8(float f) {
	return std::round(f * 255.0f); // ! Ignores overflow
};

inline float u8tof(uint32_t c) {
	return (float)c / 255.0f; // ! Ignores overflow
};

} // namespace sgd
