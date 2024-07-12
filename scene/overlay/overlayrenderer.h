#pragma once

#include "overlay.h"

#include <graphics/exports.h>

namespace sgd {

SGD_SHARED(OverlayRenderer);

struct OverlayRenderer : Shared {
	SGD_OBJECT_TYPE(OverlayRenderer, Shared);

	void add(COverlay* skybox);
	void remove(COverlay* skybox);

	void update(CVec3r eye);
	void render(RenderQueue* rq) const;

private:
	Vector<COverlayPtr> m_overlays;
};

}
