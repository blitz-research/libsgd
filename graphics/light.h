#pragma once

#include "entity.h"

namespace sgd {

SGD_SHARED(Light);

struct Light : Entity {
	SGD_OBJECT_TYPE(Light, Entity);

	Property<Vec4f> color{Vec4f(1, 1, 1, 1)};
	Property<float> range{25};
	Property<float> falloff{1};
};

} // namespace sgd
