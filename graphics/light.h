#pragma once

#include "entity.h"

namespace sgd {

SGD_SHARED(Light);

struct Light : Entity {
	SGD_OBJECT_TYPE(Light, Entity);

	Light() = default;

	explicit Light(const Light* that);

	Property<Vec4f> color{Vec4f(1, 1, 1, 1)};
	Property<float> range{25};
	Property<float> falloff{1};

private:
	Light* onCopy() const override;
};

} // namespace sgd
