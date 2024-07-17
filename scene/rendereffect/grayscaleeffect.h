#pragma once

#include "rendereffect.h"

namespace sgd {

SGD_SHARED(GrayscaleEffect);

struct GrayscaleEffect : RenderEffect {
	SGD_OBJECT_TYPE(GrayscaleEffect, RenderEffect);

	GrayscaleEffect();

private:
	BindGroupPtr m_bindGroup;
	mutable wgpu::RenderPipeline m_pipeline;

	void onValidate() override;

	void onRender(RenderContext* rc, BindGroup* sceneBindings) const override;
};

}
