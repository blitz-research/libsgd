#pragma once

#include "entity.h"

namespace sgd {

SGD_SHARED(Light);

enum struct LightType {
	undefined,
	directional,
	point,
	spot,
};

struct Light : Entity {
	SGD_OBJECT_TYPE(Light, Entity);

	Light() = default;

	explicit Light(const Light* that);

	explicit Light(LightType type) : m_type(type) {
	}

	LightType type() const {
		return m_type;
	}

	Property<Vec4f> color{Vec4f(1)};
	Property<float> range{10};
	Property<float> falloff{1};
	Property<float> innerConeAngle{0};
	Property<float> outerConeAngle{45};
	Property<bool> castsShadow{false};

private:
	LightType m_type{LightType::directional};

	Light* onCopy() const override;
};

} // namespace sgd
