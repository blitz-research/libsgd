#pragma once

#include <geom/exports.h>

namespace sgd {

struct Contact {
	Vec3r point; // sort of redundant, should equal ray.d * time.
	Vec3r normal;
	real time{};

	Contact() = default;

	Contact(real time) : time(time) {
	}
};
using CContact = const Contact&;

} // namespace sgd
