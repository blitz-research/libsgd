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
	uint32_t lodLevels;
	uint32_t quadsPerTile;
//	float meshScale;
//	float heightScale;
};
using CTerrainUniforms = const TerrainUniforms;

extern const BindGroupDescriptor terrainBindGroupDescriptor;

} // namespace sgd
