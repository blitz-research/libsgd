#pragma once

#include "renderpipeline.h"
#include "rendercontext.h"

#include <dawn/exports.h>

namespace sgd {

SGD_SHARED(Renderer);

RenderPassType renderPassType(BlendMode blendMode);

struct Renderer : GraphicsResource {

	Property<bool> enabled{true};

	virtual void onUpdate(CVec3r eye) {
	}

	virtual void render(RenderContext* rc) const {
		auto& ops = m_renderOps[(int)rc->renderPassType()];
		if(ops.empty()) return;

		rc->render(ops);
	}

protected:
	mutable Array<Vector<RenderOp>, renderPassTypeCount> m_renderOps;

	Renderer() = default;
};

inline RenderPassType renderPassType(BlendMode blendMode) {
	switch (blendMode) {
	case BlendMode::undefined:
		return RenderPassType::shadow;
	case BlendMode::opaque:
	case BlendMode::alphaMask:
		return RenderPassType::opaque;
	case BlendMode::alphaBlend:
	case BlendMode::additive:
	case BlendMode::multiply:
		return RenderPassType::blend;
	}
	unreachable();
}

} // namespace sgd
