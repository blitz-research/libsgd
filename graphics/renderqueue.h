#pragma once

#include "renderer.h"

namespace sgd {

SGD_SHARED(RenderQueue);

struct RenderQueue : Shared {
	SGD_OBJECT_TYPE(RenderQeueue, Shared);

	void clearRenderers();

	void addRenderer(Renderer* renderer);

	void render(GraphicsContext* gc);

private:
	Vector<RendererPtr> m_renderers;
};


}
