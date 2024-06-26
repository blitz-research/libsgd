#pragma once

#include "entity.h"

namespace sgd {

SGD_SHARED(DrawList);
SGD_SHARED(Overlay);

struct Overlay : Entity {
	SGD_OBJECT_TYPE(Overlay, Entity);

	Overlay() = default;

	explicit Overlay(const Overlay*);

	DrawList* drawList() const {
		return m_drawList;
	}

private:
	DrawListPtr m_drawList;
	Vec2u m_size;

	void onCreate() override;
	void onDestroy() override;
	void onShow() override;
	void onHide() override;

	Overlay* onCopy() const override;
};

} // namespace sgd
