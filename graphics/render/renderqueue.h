#pragma once

#include "../core/bindgroup.h"
#include "../material/material.h"
#include "renderop.h"

namespace sgd {

SGD_SHARED(RenderQueue);

struct RenderQueue : Shared {
	SGD_OBJECT_TYPE(RenderQueue, Shared);

	void clear();

	void addRenderOp(CBuffer* vbuffer,		 //
					 CBuffer* ibuffer,		 //
					 CMaterial* material,	 //
					 CBindGroup* geometry,	 //
					 CBindGroup* renderer,	 //
					 uint32_t elementCount,	 //
					 uint32_t instanceCount, //
					 uint32_t firstElement,
					 bool shadows); //

	CVector<RenderOp> renderOps(RenderPassType rpassType) const {
		return m_renderOps[(int)rpassType];
	}

private:
	Array<Vector<RenderOp>, numRenderPassTypes> m_renderOps;
};

} // namespace sgd
