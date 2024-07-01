#include "renderer.h"

#include "material.h"

namespace sgd {

RenderPassType renderPassType(BlendMode blendMode) {
	switch (blendMode) {
	case BlendMode::opaque:
	case BlendMode::alphaMask:
		return RenderPassType::opaque;
	case BlendMode::alphaBlend:
		return RenderPassType::blend;
	default:
		SGD_ABORT();
	}
}

void Renderer::addRenderOp(GraphicsContext* gc,		//
						   CMaterial* material,		//
//						   CBuffer* instanceBuffer, //
						   CBuffer* vertexBuffer,	//
						   CBuffer* indexBuffer,	//
						   CBindGroup* renderer,	//
						   uint32_t elementCount,	//
						   uint32_t instanceCount,	//
						   uint32_t firstElement,	//
						   bool shadowsEnabled) const {

	{
		auto rpassType = renderPassType(material->blendMode());
		auto pipeline = getOrCreateRenderPipeline(gc, rpassType, material, renderer);
		m_renderOps[(int)rpassType].emplace_back( //
			vertexBuffer,						  //
			indexBuffer,						  //
			material->bindGroup(),				  //
			renderer,							  //
			pipeline,							  //
			elementCount,						  //
			instanceCount,						  //
			firstElement);
	}

	if (material->blendMode() == BlendMode::alphaMask) {
#if 0
		auto rpassType = RenderPassType::blend;
		auto pipeline = getOrCreateRenderPipeline(gc, rpassType, material, renderer);
		m_renderOps[(int)rpassType].emplace_back( //
			vertexBuffer,						  //
			indexBuffer,						  //
			material->bindGroup(),				  //
			renderer,							  //
			pipeline,							  //
			elementCount,						  //
			instanceCount,						  //
			firstElement);
#endif
	}

	if (shadowsEnabled) {
		SGD_ASSERT(material->blendMode() == BlendMode::opaque); // TODO: || material->blendMode() == BlendMode::alphaMask);
		auto rpassType = RenderPassType::shadow;
		auto spipeline = getOrCreateRenderPipeline(gc, rpassType, material, renderer);
		m_renderOps[(int)rpassType].emplace_back( //
			vertexBuffer,						  //
			indexBuffer,						  //
			material->bindGroup(),				  //
			renderer,							  //
			spipeline,							  //
			elementCount,						  //
			instanceCount,						  //
			firstElement);
	}
}

} // namespace sgd
