#pragma once

#include "entity.h"
#include <graphics/exports.h>

namespace sgd {

SGD_SHARED(Sprite);

struct Sprite : Entity {
	SGD_OBJECT_TYPE(Sprite, Entity);

	Sprite() = default;

	explicit Sprite(const Sprite* that);

	Property<ImagePtr> image;

	Property<Vec4f> color{Vec4f(1)};

	Property<float> frame;

private:
	void onCreate() override;
	void onShow() override;
	void onHide() override;

	Entity* onCopy() const override;
};

} // namespace sgd
