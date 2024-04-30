#pragma once

#include "entity.h"

namespace sgd {

SGD_SHARED(Camera);

enum struct CameraType { undefined, perspective, orthographic };

struct Camera : Entity {
	SGD_OBJECT_TYPE(Camera, Entity);

	Camera() = default;

	explicit Camera(const Camera* that);

	explicit Camera(CameraType type) : m_type(type) {
	}

	Property<float> fov {45};
	Property<float> near{.1};
	Property<float> far{100};

private:
	CameraType m_type{CameraType::perspective};

	Entity* onCopy() const override;
};

} // namespace sgd
