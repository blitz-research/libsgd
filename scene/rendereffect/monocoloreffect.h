#pragma once

#include "rendereffect.h"

namespace sgd {

SGD_SHARED(MonocolorEffect);

struct MonocolorEffect : RenderEffect {
	SGD_OBJECT_TYPE(MonocolorEffect, RenderEffect);

	MonocolorEffect();

	Property<Vec4f> color{Vec4f(1)};

private:
	BindGroupPtr m_bindGroup;
	mutable wgpu::RenderPipeline m_pipeline;

	void onValidate() override;

	void onRender(RenderContext* rc, BindGroup* sceneBindings) const override;
};

}
