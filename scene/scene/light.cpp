#include "light.h"

#include "scenerenderer.h"

namespace sgd {

Light::Light(const Light* that)
	: Entity(that),				//
	  color(that->color()),		//
	  range(that->range()),		//
	  falloff(that->falloff()), //
	  innerConeAngle(that->innerConeAngle()),
	  outerConeAngle(that->outerConeAngle()),
	  shadowsEnabled(that->shadowsEnabled()),
	  priority(that->priority()),
	  m_type(that->m_type) {
}

Light* Light::onCopy() const {
	return new Light(this);
}

void Light::onShow() {
	scene()->sceneRenderer()->add(this);
}

void Light::onHide() {
	scene()->sceneRenderer()->remove(this);
}

} // namespace sgd
