#pragma once

#include <cassert>
#include <vector>
#include <functional>
#include <cinttypes>

#include <sgd/sgd.h>

namespace skirmish {

// Some useful constants.
constexpr auto pi = 3.14159265359;
constexpr auto twoPi = pi * 2.0;
constexpr auto halfPi = pi * .5;
constexpr auto degreesToRadians = .01745329252;
constexpr auto radiansToDegrees = 57.295779513;

// Seed rnd generator with static seed.
void seedRnd(uint32_t seed);

// Seed rnd generator with current time().
void seedRnd();

//! Generate a pseudo random number from [0, 1).
float rnd();

//! Generate a pseudo random number from [0, max).
float rnd(float max);

//! Generate a pseudo random number from [min, max).
float rnd(float min, float max);

//! Same as mod but handles negative x value so there's no 'hole' around 0, y should still be positive.
inline int floorMod(int x, int y) {
	return x >= 0 ? x % y : x - ((x - y + 1) / y) * y;
}

//! Same as std::fmod but handles negative x value so there's no 'hole' around 0, y should still be positive.
inline float floorMod(float x, float y) {
	return x - std::floor(float(x) / float(y)) * y;
}

}
