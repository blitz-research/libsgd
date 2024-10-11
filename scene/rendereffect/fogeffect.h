#pragma once

#include "rendereffect.h"

namespace sgd {

SGD_SHARED(FogEffect);

struct FogEffect : RenderEffect {
	SGD_OBJECT_TYPE(FogEffect, RenderEffect);

	FogEffect();

	Property<Vec4f> color{Vec4f(.8f, .9f, 1, 1)};
	Property<float> near{.1};
	Property<float> far{1000};
	Property<float> power{2};

private:
	BindGroupPtr m_bindGroup;
	TexturePtr m_renderTarget;
	mutable wgpu::RenderPipeline m_pipeline;

	Texture* onValidate(Texture* sourceTexture, Texture* depthBuffer) override;

	void onRender(RenderContext* rc, BindGroup* sceneBindings) const override;
};

}
