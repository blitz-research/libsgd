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
	uint32_t size{1024};
	uint32_t lods{4};
	uint32_t quadsPerTile{16};
	uint32_t debugMode{0};
	float materialTexelSize{1.0 / 1024.0f};
};
using CTerrainUniforms = const TerrainUniforms;

extern const BindGroupDescriptor terrainBindGroupDescriptor;

} // namespace sgd
