#pragma once

#include "../scene/entity.h"

namespace sgd {

SGD_SHARED(DrawList);

SGD_SHARED(Overlay);

struct Overlay : Entity {
	SGD_OBJECT_TYPE(Overlay, Entity);

	Overlay();
	explicit Overlay(const Overlay*);

	DrawList* drawList() const {
		return m_drawList;
	}

private:
	DrawListPtr m_drawList;

	Overlay* onCopy() const override;

	void onShow() override;
	void onHide() override;
};

} // namespace sgd
