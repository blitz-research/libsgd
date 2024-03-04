#include "light.h"

namespace sgd {

Light::Light(const Light* that)
	: Entity(that),		 //
	  color(that->color()), //
	  range(that->range()), //
	  falloff(that->falloff()) {
}

Light* Light::onCopy() const {
	return new Light(this);
}

} // namespace sgd
