#pragma once

#include "rendereffect.h"

namespace sgd {

SGD_SHARED(BlurEffect);

struct BlurEffect : RenderEffect {
	SGD_OBJECT_TYPE(BlurEffect, RenderEffect);

	BlurEffect();

	Property<uint32_t> kernelSize{5};

private:
	TexturePtr m_renderTargets[2];
	BindGroupPtr m_bindGroups[2];
	mutable wgpu::RenderPipeline m_pipeline;

	void onValidate() override;

	void onRender(RenderContext* rc, BindGroup* sceneBindings) const override;
};

}
