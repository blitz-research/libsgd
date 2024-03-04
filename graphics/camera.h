#pragma once

#include "entity.h"

namespace sgd {

SGD_SHARED(Camera);

struct Camera : Entity {
	SGD_OBJECT_TYPE(Camera, Entity);

	Camera() = default;

	explicit Camera(const Camera* that);

	Property<float> fovY{45};
	Property<float> near{.1};
	Property<float> far{100};

private:
	Entity* onCopy() const override;
};

}
