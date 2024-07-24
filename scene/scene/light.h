#pragma once

#include "entity.h"

namespace sgd {

SGD_SHARED(Light);

enum struct LightType {
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

	Property<Vec4f> color{Vec4f(1)};
	Property<float> range{10};
	Property<float> falloff{1};
	Property<float> innerConeAngle{0};
	Property<float> outerConeAngle{45};
	Property<bool> shadowsEnabled{false};
	Property<int> priority{0};

	LightType type() const {
		return m_type;
	}

private:
	LightType m_type{LightType::directional};

	Light* onCopy() const override;

	void onShow() override;
	void onHide() override;
};

} // namespace sgd
