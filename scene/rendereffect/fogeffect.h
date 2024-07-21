#pragma once

#include "rendereffect.h"

namespace sgd {

SGD_SHARED(FogEffect);

struct FogEffect : RenderEffect {
	SGD_OBJECT_TYPE(FogEffect, RenderEffect);

	FogEffect();

	Property<Vec4f> color{Vec4f(.8f, .9f, 1, 1)};
	Property<float> near{0};
	Property<float> far{1024.0f};
	Property<float> power{2.0f};

private:
	BindGroupPtr m_bindGroup;
	TexturePtr m_renderTarget;
	mutable wgpu::RenderPipeline m_pipeline;

	Texture* onValidate(Texture* sourceTexture, Texture* depthBuffer) override;

	void onRender(RenderContext* rc, BindGroup* sceneBindings) const override;
};

}
