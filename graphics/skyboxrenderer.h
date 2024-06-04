#pragma once

#include "renderer.h"

namespace sgd {

SGD_SHARED(SkyboxRenderer);

struct SkyboxRenderer : Renderer {
	SGD_OBJECT_TYPE(SkyboxRenderer, Renderer);

	SkyboxRenderer();

	Property<TexturePtr> skyTexture;

	Property<AffineMat4r> worldMatrix;

	Property<float> roughness;

private:
	MaterialPtr m_material;
	BindGroupPtr m_bindGroup;
	BufferPtr m_uniformBuffer;

	mutable bool m_rebuildRenderOps{true};
	mutable bool m_updateUniforms{true};

	void onValidate(GraphicsContext* gc) const override;
};

}
