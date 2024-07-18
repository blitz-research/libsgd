#include "overlay.h"

#include "../scene/scenerenderer.h"
#include "overlayrenderer.h"

namespace sgd {

Overlay::Overlay() : m_drawList(new DrawList()) {
}

Overlay::Overlay(const Overlay* that) : Entity(that), m_drawList(that->m_drawList) {
}

Overlay* Overlay::onCopy() const {
	return new Overlay(this);
}

void Overlay::onShow() {

	auto resize = [=](CVec2u size){ //
		m_drawList->projectionMatrix = Mat4f::ortho(0, (float)size.x, (float)size.y, 0, 0, 1);
	};

	scene()->sceneRenderer()->renderTargetSizeChanged.connect(this, [=](CVec2u size) { resize(size);});

	resize(scene()->sceneRenderer()->renderTargetSize());

	scene()->sceneRenderer()->overlayRenderer()->add(this);
}

void Overlay::onHide() {
	scene()->sceneRenderer()->overlayRenderer()->remove(this);

	scene()->sceneRenderer()->renderTargetSizeChanged.disconnect(this);
}

} // namespace sgd
