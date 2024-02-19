#include "renderqueue.h"

namespace sgd {

void RenderQueue::clearRenderers() {

	m_renderers.clear();
}

void RenderQueue::addRenderer(Renderer* renderer){

	m_renderers.push_back(renderer);
}

void RenderQueue::render(GraphicsContext* gc) {

	for (RenderPass rpass : {RenderPass::clear, RenderPass::opaque, RenderPass::blend}) {

		gc->beginRenderPass(rpass);

		for (auto r : m_renderers) r->render(gc);

		gc->endRenderPass();
	}
}


}
