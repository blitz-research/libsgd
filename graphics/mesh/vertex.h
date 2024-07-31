#pragma once

#include <geom/exports.h>

namespace sgd {

struct Vertex {
	Vec3f position;
	Vec3f normal;
	Vec4f tangent;
	Vec3f texCoords;
	Vec4f color{1};
	uint8_t joints[4]{};
	float weights[4]{};

	Vertex() = default;

	Vertex(CVec3f position, CVec3f normal, CVec2f texCoords) : position(position), normal(normal), texCoords(texCoords, 0) {
	}

	Vertex(CVec3f position, CVec2f texCoords, CVec4f color) : position(position), texCoords(texCoords, 0), color(color) {
	}
};
using CVertex = const Vertex&;

}
