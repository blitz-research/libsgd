#pragma once

#include "std.h"

namespace sgd {

// Seed rnd generator
void seedRnd(uint32_t seed);

// Seed rnd generator with current time().
void seedRnd();

//! Generate a pseudo random number from [0, 1).
float rnd();

//! Generate a pseudo random number from [0, max).
float rnd(float max);

//! Generate a pseudo random number from [min, max).
float rnd(float min, float max);

} // namespace xros
