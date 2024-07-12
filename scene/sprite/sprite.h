#pragma once

#include "../scene/entity.h"

namespace sgd {

SGD_SHARED(Image);

SGD_SHARED(Sprite);

struct Sprite : Entity {
	SGD_OBJECT_TYPE(Sprite, Entity);

	Sprite() = default;
	explicit Sprite(Image* image);
	explicit Sprite(const Sprite* that);

	Property<ImagePtr> image;
	Property<Vec4f> color{Vec4f(1)};
	Property<float> frame;

private:
	Entity* onCopy() const override;
	void onShow() override;
	void onHide() override;
};

} // namespace sgd
