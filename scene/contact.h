#pragma once

#include <geom/exports.h>

namespace sgd {

struct Contact {
	Vec3r point; // sort of redundant, should equal ray.d * time.
	Vec3r normal;
	real time{};

	Contact() = default;

	explicit Contact(real time) : time(time) {
	}
};
using CContact = const Contact&;

inline std::ostream& operator<<(std::ostream& os, const Contact& c) {
	return os << '(' << c.point << ',' << c.normal << ',' << c.time << ')';
}

} // namespace sgd
