#include "planerenderer.h"

#include "planeentity.h"

#include <graphics/exports.h>

namespace sgd {

void PlaneRenderer::add(CPlaneEntity* plane) {
	m_planes.emplace_back(plane);
}

void PlaneRenderer::remove(CPlaneEntity* plane) {
	sgd::remove(m_planes, plane);
}

void PlaneRenderer::update(CVec3r eye) {
	for (CPlaneEntity* tp : m_planes) {
		auto matrix = tp->worldMatrix();
		matrix.t -= eye;
		tp->bindings()->worldMatrix = matrix;
	}
}

void PlaneRenderer::render(RenderQueue* rq) const {
	for (CPlaneEntity* tp : m_planes) {
		tp->bindings()->render(rq);
	}
}

} // namespace sgd
