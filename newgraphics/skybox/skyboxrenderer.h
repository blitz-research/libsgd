#pragma once

#include "../core/bindgroup.h"

namespace sgd {

SGD_SHARED(Material);
SGD_SHARED(RenderContext);

SGD_SHARED(SkyboxRenderer);

struct SkyboxRenderer : GraphicsResource {
	SGD_OBJECT_TYPE(SkyboxRenderer, GraphicsResource);

	SkyboxRenderer();

	Property<TexturePtr> skyTexture;
	Property<AffineMat4r> worldMatrix;
	Property<float> roughness;

	void render(RenderContext* rc) const;

private:
	MaterialPtr m_material;
	BufferPtr m_uniformBuffer;
	BindGroupPtr m_bindGroup;

	mutable bool m_rebuildRenderOps{true};
	mutable bool m_updateUniforms{true};

	void onValidate() const override;
};

} // namespace sgd
