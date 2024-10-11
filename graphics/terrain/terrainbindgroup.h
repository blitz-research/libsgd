#pragma once

#include "../core/bindgroup.h"

namespace sgd {

struct TerrainVertex {
	Vec3f position;
	uint32_t color{};

	TerrainVertex() = default;
	TerrainVertex(CVec3f position, uint32_t color) : position(position), color(color) {
	}
};
using CTerrainVertex = const TerrainVertex;

struct alignas(16) TerrainUniforms {
	Mat4f worldMatrix;
	float size{1024};
	uint32_t lods{4};
	float quadsPerTile{16};
	float materialTexelSize{1.0 / 1024.0f};
	float heightTexelSize{1.0 / 1024.0};
	uint32_t debugMode{0};
};
using CTerrainUniforms = const TerrainUniforms;

extern const BindGroupDescriptor terrainBindGroupDescriptor;

} // namespace sgd
