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
	TexturePtr m_renderTarget;
	mutable wgpu::RenderPipeline m_pipeline;

	Texture* onValidate(Texture* sourceTexture) override;

	void onRender(RenderContext* rc, BindGroup* sceneBindings) const override;
};

}
