#include "camera.h"

#include "scenerenderer.h"

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

void Camera::onShow() {
	scene()->sceneRenderer()->renderTargetSizeChanged.connect(this, [=](CVec2u size) { //
		viewportSize = size;
	});
	viewportSize = scene()->sceneRenderer()->renderTargetSize();
	scene()->sceneRenderer()->add(this);
}

void Camera::onHide() {
	scene()->sceneRenderer()->remove(this);
	scene()->sceneRenderer()->renderTargetSizeChanged.disconnect(this);
}

Mat4f Camera::projectionMatrix() const {
	if (!m_projDirty) return m_projMatrix;
	m_projDirty = false;

	auto aspect = (float)viewportSize().x / (float)viewportSize().y;

	switch (m_type) {
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
	return (wCoords * Vec2f(.5f, -.5f) + .5f) * Vec2f(camera->viewportSize());
}

Vec3r unproject(Camera* camera, CVec2f p, float d) {

	// NDC space on near plane
	auto ndc = Vec4f((p / Vec2f(camera->viewportSize()) - 0.5f) * Vec2f(2.0f, -2.0f), 0, 1);

	// To 3d view space coords
	auto tv = camera->inverseProjectionMatrix() * ndc;
	auto vsCoords = tv.xyz() / tv.w;

	// to z=d
	vsCoords *= d / vsCoords.z;

	return camera->worldMatrix() * Vec3r(vsCoords);
}

} // namespace sgd
