#pragma once

#include "rendercontext.h"
#include "renderpipeline.h"

#include <dawn/exports.h>

namespace sgd {

SGD_SHARED(Renderer);

struct Renderer : GraphicsResource {

	Property<bool> enabled{true};

	virtual void onUpdate(CVec3r eye) {
	}

	CVector<RenderOp> renderOps(RenderPassType rpassType) const {
		return m_renderOps[(int)rpassType];
	}

	virtual void render(RenderContext* rc) const {
		auto& ops = m_renderOps[(int)rc->renderPassType()];
		if (ops.empty()) return;
		rc->render(ops);
	}

protected:
	mutable Array<Vector<RenderOp>, renderPassTypeCount> m_renderOps;

	Renderer() = default;

	void addRenderOp(GraphicsContext* gc,	  //
					 CMaterial* material,	  //
//					 CBuffer* instanceBuffer, //
					 CBuffer* vertexBuffer,	  //
					 CBuffer* indexBuffer,	  //
					 CBindGroup* renderer,	  //
					 uint32_t elementCount,	  //
					 uint32_t instanceCount,  //
					 uint32_t firstElement,	  //
					 bool shadowsEnabled) const;
};

RenderPassType renderPassType(BlendMode blendMode);

} // namespace sgd
