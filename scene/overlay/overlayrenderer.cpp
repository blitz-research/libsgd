#include "overlayrenderer.h"

namespace sgd {

void OverlayRenderer::add(COverlay* overlay) {
	m_overlays.emplace_back(overlay);
}

void OverlayRenderer::remove(COverlay* overlay) {
	sgd::remove(m_overlays, overlay);
}

void OverlayRenderer::update(CVec3r eye) {
	for (COverlay* overlay : m_overlays) {
		auto drawList = overlay->drawList();
		drawList->flush();
	}
}

void OverlayRenderer::render(RenderQueue* rq) const {
	for (COverlay* overlay : m_overlays) {
		auto drawList = overlay->drawList();
		drawList->render(rq);
	}
}

}
