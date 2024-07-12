#include "camera.h"

#include "scene.h"

namespace sgd {

void Camera::init() {
	viewportSize.changed.connect(nullptr, [=](CVec2u) { m_projDirty = m_invProjDirty = true; });
	fov.changed.connect(nullptr, [=](float) { m_projDirty = m_invProjDirty = true; });
	near.changed.connect(nullptr, [=](float) { m_projDirty = m_invProjDirty = true; });
	far.changed.connect(nullptr, [=](float) { m_projDirty = m_invProjDirty = true; });
}

Camera::Camera(CameraType type) : m_type(type) {
	init();
}

Camera::Camera(const Camera* that)
	: Entity(that),						  //
	  m_type(that->m_type),				  //
	  viewportSize(that->viewportSize()), //
	  fov(that->fov()),					  //
	  near(that->near()),				  //
	  far(that->far()) {				  //
	init();
}

Entity* Camera::onCopy() const {
	return new Camera(this);
}

Mat4f Camera::projectionMatrix() const {
	if (!m_projDirty) return m_projMatrix;
	m_projDirty = false;

	auto aspect = (float)viewportSize().x / (float)viewportSize().y;

	switch (m_type) {
	case CameraType::undefined:
	case CameraType::perspective:
		return m_projMatrix = Mat4f::perspective(fov(), aspect, near(), far());
	case CameraType::orthographic:
		return m_projMatrix = Mat4f::orthographic(fov(), aspect, near(), far());
	}
	return {};
}

Mat4f Camera::inverseProjectionMatrix() const {
	if (!m_invProjDirty) return m_invProjMatrix;
	m_invProjDirty = false;
	return m_invProjMatrix = inverse(projectionMatrix());
}

Expected<Vec2f, bool> project(Camera* camera, CVec3r p) {
	// world space -> view space
	auto vp = Vec4f(inverse(camera->worldMatrix()) * p, 1);

	// view space -> clip space
	auto clipCoords = camera->projectionMatrix() * vp;
	if (clipCoords.w <= 0) return false;

	// clip space -> NDC space
	auto wCoords = clipCoords.xy() / clipCoords.w;

	// NDC space -> window coords
	return (wCoords * Vec2f(.5f, -.5f) + .5f) * Vec2f(camera->scene()->viewportSize());
}

} // namespace sgd
