#include "light.h"

namespace sgd {

Light::Light(const Light* that)
	: Entity(that),				//
	  color(that->color()),		//
	  range(that->range()),		//
	  falloff(that->falloff()), //
	  m_type(that->m_type) {
}

Light* Light::onCopy() const {
	return new Light(this);
}

} // namespace sgd
