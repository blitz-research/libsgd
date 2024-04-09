#pragma once

#include "renderpipeline.h"
#include "rendercontext.h"

#include <dawn/exports.h>

namespace sgd {

SGD_SHARED(Renderer);

RenderPassType renderPassType(BlendMode blendMode);

struct Renderer : GraphicsResource {

	Property<bool> enabled{true};

	virtual void onUpdate(CVec3f eye) {
	}

	virtual void render(RenderContext* rc) const {
		auto& ops = m_renderOps[(int)rc->renderPassType()];
		if(ops.empty()) return;

		if(rc->renderPassType()==RenderPassType::shadow) {
//			log() << "### rendering shadows";
		}
		rc->render(ops);
	}

protected:
	mutable Array<Vector<RenderOp>, renderPassTypeCount> m_renderOps;

	Renderer() = default;
};

inline RenderPassType renderPassType(BlendMode blendMode) {
	switch (blendMode) {
	case BlendMode::undefined:
	case BlendMode::opaque:
		return RenderPassType::opaque;
	case BlendMode::alpha:
	case BlendMode::additive:
	case BlendMode::multiply:
		return RenderPassType::blend;
	}
	unreachable();
}

} // namespace sgd
