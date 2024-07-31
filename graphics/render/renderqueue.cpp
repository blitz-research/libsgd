#include "renderqueue.h"

namespace sgd {

void RenderQueue::clear() {
	m_renderOps = {};
}

void RenderQueue::addRenderOp(CBuffer* vbuffer,		  //
							  CBuffer* ibuffer,		  //
							  CMaterial* material,	  //
							  CBindGroup* geometry,	  //
							  CBindGroup* renderer,	  //
							  uint32_t elementCount,  //
							  uint32_t instanceCount, //
							  uint32_t firstElement,  //
							  uint32_t firstInstance, //
							  bool shadows) {

	auto blendMode = material->blendMode();
	auto rpassType =
		(blendMode == BlendMode::opaque || blendMode == BlendMode::alphaMask) ? RenderPassType::opaque : RenderPassType::blend;

	m_renderOps[(int)rpassType].emplace_back(								//
		vbuffer ? vbuffer->wgpuBuffer() : nullptr,							//
		ibuffer ? ibuffer->wgpuBuffer() : nullptr,							//
		material->wgpuBindGroup(),											//
		geometry->wgpuBindGroup(),											//
		renderer->wgpuBindGroup(),											//
		getOrCreateRenderPipeline(rpassType, material, geometry, renderer), //
		elementCount,														//
		instanceCount,														//
		firstElement,														//
		firstInstance);														//

	if (shadows && rpassType == RenderPassType::opaque) {
		m_renderOps[(int)RenderPassType::shadow].emplace_back(m_renderOps[(int)RenderPassType::opaque].back());
		m_renderOps[(int)RenderPassType::shadow].back().pipeline =
			getOrCreateRenderPipeline(RenderPassType::shadow, material, geometry, renderer);
	}
}

} // namespace sgd
