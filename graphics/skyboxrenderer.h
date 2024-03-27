#pragma once

#include "renderer.h"

namespace sgd {

SGD_SHARED(SkyboxRenderer);

struct SkyboxRenderer : Renderer {
	SGD_OBJECT_TYPE(SkyboxRenderer, Renderer);

	SkyboxRenderer();

	Property<TexturePtr> skyTexture;

	Property<AffineMat4f> worldMatrix;

	Property<float> roughness;

private:
	BindGroupPtr m_bindGroup;
	BufferPtr m_uniformBuffer;

	mutable wgpu::RenderPipeline m_pipeline;

	void onValidate(GraphicsContext* gc) const override;

	void onRender(GraphicsContext* gc) const override;
};

}
