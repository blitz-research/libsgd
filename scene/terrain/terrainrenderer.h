#pragma once

#include "terrain.h"

namespace sgd {

SGD_SHARED(RenderQueue);

SGD_SHARED(TerrainRenderer);

struct TerrainRenderer : Shared {
	SGD_OBJECT_TYPE(TerrainRenderer, Shared);

	void add(CTerrain* terrain);
	void remove(CTerrain* terrain);

	void update(CVec3r eye);
	void render(RenderQueue* rq) const;

private:
	Vector<CTerrainPtr> m_terrains;
};

}
