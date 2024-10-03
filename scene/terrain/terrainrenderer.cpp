#include "terrainrenderer.h"

#include <graphics/exports.h>

namespace sgd {

void TerrainRenderer::add(CTerrain* terrain) {
	m_terrains.emplace_back(terrain);
}

void TerrainRenderer::remove(CTerrain* terrain) {
	sgd::remove(m_terrains, terrain);
}

void TerrainRenderer::update(CVec3r eye) {
}

void TerrainRenderer::render(RenderQueue* rq) const {
	for(CTerrain* tp : m_terrains) {
		tp->bindings()->render(rq);
	}
}

}
