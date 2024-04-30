#pragma once

#include "entity.h"

namespace sgd {

SGD_SHARED(Material);
SGD_SHARED(Sprite);

struct Sprite : Entity {
	SGD_OBJECT_TYPE(Sprite, Entity);

	Sprite() = default;

	explicit Sprite(const Sprite* that);

	Property<MaterialPtr> material;
	Property<Vec4f> color{Vec4f(1)};
	Property<Rectf> rect{Rectf(-.5f, .5f)};

private:
	void onCreate() override;
	void onShow() override;
	void onHide() override;

	Entity* onCopy() const override;
};

} // namespace sgd
