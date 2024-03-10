#include "camera.h"

namespace sgd {

Camera::Camera(const Camera* that)
	: Entity(that),		  //
	  fovY(that->fovY()), //
	  near(that->near()), //
	  far(that->far()),	  //
	  m_type(that->m_type) {
}

Entity* Camera::onCopy() const {
	return new Camera(this);
}

} // namespace sgd
