#pragma once

#include "entity.h"

namespace sgd {

SGD_SHARED(Camera);

enum struct CameraType { perspective, orthographic };

struct Camera : Entity {
	SGD_OBJECT_TYPE(Camera, Entity);

	explicit Camera(CameraType type);
	explicit Camera(const Camera* that);

	Property<Vec2u> viewportSize{Vec2u(640u, 480u)};
	Property<float> fov{90};
	Property<float> near{.1};
	Property<float> far{1000};

	CameraType type() const {
		return m_type;
	}

	Mat4f projectionMatrix() const;
	Mat4f inverseProjectionMatrix() const;

private:
	CameraType m_type{CameraType::perspective};

	mutable Mat4f m_projMatrix;
	mutable Mat4f m_invProjMatrix;
	mutable bool m_projDirty{true};
	mutable bool m_invProjDirty{true};

	void init();

	Entity* onCopy() const override;

	void onShow() override;
	void onHide() override;
};

Expected<Vec2f, bool> project(Camera* camera, CVec3r p);

Vec3r unproject(Camera* camera, CVec2f p, float d);

} // namespace sgd
