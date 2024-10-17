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

	Property<uint32_t> size{1024};
	Property<uint32_t> lods{4};
	Property<uint32_t> materialSize{1024};
	Property<int> debugMode{0};

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

	CTerrainUniforms& uniforms() const;
	TerrainUniforms& lockUniforms() const;
	void unlockUniforms() const;

	void onValidate() const override;
};

} // namespace sgd
