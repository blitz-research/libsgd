#pragma once

#include "terrainbindgroup.h"

namespace sgd {

SGD_SHARED(Material);
SGD_SHARED(RenderQueue);

SGD_SHARED(TerrainBindings);

struct TerrainBindings : GraphicsResource {
	SGD_OBJECT_TYPE(TerrainBindings, GraphicsResource);

	TerrainBindings();

	Property<AffineMat4f> worldMatrix;

	Property<uint32_t> lodLevels;

	Property<uint32_t> quadsPerTile;

	Property<CTexturePtr> heightTexture;

	Property<CTexturePtr> normalTexture;

	Property<MaterialPtr> material;

	Property<bool> shadowsEnabled;

	void render(RenderQueue* rq) const;

private:
	BindGroupPtr m_bindGroup;
	BufferPtr m_uniformBuffer;
	mutable BufferPtr m_vertexBuffer;
	mutable BufferPtr m_indexBuffer;
	mutable uint32_t m_indexCount{};

	CTerrainUniforms& uniforms() const {
		return *(CTerrainUniforms*)m_uniformBuffer->data();
	}

	TerrainUniforms& lockUniforms() {
		return *(TerrainUniforms*)m_uniformBuffer->lock();
	}
	void unlockUniforms() {
		m_uniformBuffer->unlock();
	}

	void onValidate() const override;
};

} // namespace sgd
