#pragma once

#include "rendereffect.h"

namespace sgd {

SGD_SHARED(BlurEffect);

struct BlurEffect : RenderEffect {
	SGD_OBJECT_TYPE(BlurEffect, RenderEffect);

	BlurEffect();

	Property<uint32_t> radius{2};

private:
	BindGroupPtr m_bindGroups[2];
	TexturePtr m_renderTargets[2];
	mutable wgpu::RenderPipeline m_pipeline;

	Texture* onValidate(Texture* sourceTexture, Texture* depthBuffer) override;

	void onRender(RenderContext* rc, BindGroup* sceneBindings) const override;
};

}
