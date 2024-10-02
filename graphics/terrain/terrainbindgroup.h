#pragma once

#include "../core/bindgroup.h"

namespace sgd {

struct TerrainVertex {
	Vec3f position;
	Vec3f color{};	// TODO: uint32_t-ize

	TerrainVertex() = default;
	TerrainVertex(CVec3f position, CVec3f color) : position(position), color(color) {
	}
};
using CTerrainVertex = const TerrainVertex;

struct alignas(16) TerrainUniforms {
	uint32_t lodLevels;
	uint32_t quadsPerTile;
//	float meshScale;
//	float heightScale;
};
using CTerrainUniforms = const TerrainUniforms;

extern const BindGroupDescriptor terrainBindGroupDescriptor;

} // namespace sgd
