#pragma once

#include "rendereffect.h"

namespace sgd {
SGD_SHARED(BloomEffect);

struct BloomEffect : RenderEffect {
	static constexpr int numPasses = 3;

	SGD_OBJECT_TYPE(BloomEffect, RenderEffect);

	BloomEffect();

	Property<uint32_t> radius{2};

private:
	struct Pass {
		BindGroupPtr bindGroup;
		TexturePtr renderTarget;
		mutable RenderPassType renderPassType;
		mutable wgpu::RenderPipeline pipeline;
	};

	Array<Pass, numPasses> m_passes;

	Texture* onValidate(Texture* sourceTexture, Texture* depthBuffer) override;

	void onRender(RenderContext* rc, BindGroup* sceneBindings) const override;
};

}
