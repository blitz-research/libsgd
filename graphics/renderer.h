#pragma once

#include "renderpipeline.h"

#include <dawn/exports.h>

namespace sgd {

SGD_SHARED(Renderer);

struct Renderer : GraphicsResource {

	int renderPassMask() const {return m_renderPassMask;}

	void render(GraphicsContext* gc) const {
		if(!(1 << (int)gc->renderPass() & m_renderPassMask)) return;
		onRender(gc);
	}

	virtual void onUpdate() const {}

protected:
	mutable int m_renderPassMask{};

	virtual void onRender(GraphicsContext* gc) const = 0;
};

}
