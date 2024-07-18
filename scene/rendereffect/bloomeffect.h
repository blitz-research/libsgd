#pragma once

#include "rendereffect.h"

namespace sgd {
SGD_SHARED(BloomEffect);

struct BloomEffect : RenderEffect {
	SGD_OBJECT_TYPE(BloomEffect, RenderEffect);

	BloomEffect();

private:
	BindGroupPtr m_bindGroups[3];
	TexturePtr m_renderTargets[3];
	mutable wgpu::RenderPipeline m_pipeline;

	Texture* onValidate(Texture* sourceTexture) override;

	void onRender(RenderContext* rc, BindGroup* sceneBindings) const override;
};

}
