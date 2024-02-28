#pragma once

#include "renderpipeline.h"

#include <dawn/exports.h>

namespace sgd {

SGD_SHARED(Renderer);

struct Renderer : GraphicsResource {

	Property<bool> enabled{true};

	virtual void onUpdate() const {
	}

	void render(GraphicsContext* gc) const {
		if (!(1 << (int)gc->renderPass() & m_renderPassMask)) return;
		onRender(gc);
	}

protected:
	mutable int m_renderPassMask{~0};

	Renderer() = default;

	virtual void onRender(GraphicsContext* gc) const = 0;
};

} // namespace sgd
