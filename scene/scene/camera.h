#pragma once

#include "entity.h"

namespace sgd {

SGD_SHARED(Camera);

enum struct CameraType { undefined, perspective, orthographic };

struct Camera : Entity {
	SGD_OBJECT_TYPE(Camera, Entity);

	explicit Camera(CameraType type);
	explicit Camera(const Camera* that);

	Property<Vec2u> viewportSize{Vec2u(640u, 480u)};
	Property<float> fov{45};
	Property<float> near{.125f};
	Property<float> far{1024.0f};

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
};

Expected<Vec2f, bool> project(Camera* camera, CVec3r p);

} // namespace sgd
