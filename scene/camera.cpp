#include "camera.h"

namespace sgd {

void Camera::init() {
	fov.changed.connect(nullptr, [=](float) { m_projDirty = m_invProjDirty = true; });
	aspect.changed.connect(nullptr, [=](float) { m_projDirty = m_invProjDirty = true; });
	near.changed.connect(nullptr, [=](float) { m_projDirty = m_invProjDirty = true; });
	far.changed.connect(nullptr, [=](float) { m_projDirty = m_invProjDirty = true; });
}

Camera::Camera(CameraType type) : m_type(type) {
	init();
}

Camera::Camera(const Camera* that)
	: Entity(that),							  //
	  m_type(that->m_type), fov(that->fov()), //
	  aspect(that->aspect()),				  //
	  near(that->near()),					  //
	  far(that->far()) {					  //
	init();
}

Entity* Camera::onCopy() const {
	return new Camera(this);
}

Mat4f Camera::projectionMatrix() const {
	if (!m_projDirty) return m_projMatrix;
	m_projDirty = false;
	switch (m_type) {
	case CameraType::undefined:
	case CameraType::perspective:
		return m_projMatrix = Mat4f::perspective(fov(), aspect(), near(), far());
	case CameraType::orthographic:
		return m_projMatrix = Mat4f::orthographic(fov(), aspect(), near(), far());
	}
}

Mat4f Camera::inverseProjectionMatrix() const {
	if (!m_invProjDirty) return m_invProjMatrix;
	m_invProjDirty = false;
	SGD_LOG << "invProjMatrix";
	return m_invProjMatrix = inverse(projectionMatrix());
}

} // namespace sgd
