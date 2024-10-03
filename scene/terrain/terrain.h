#pragma once

#include "../scene/entity.h"

namespace sgd {

SGD_SHARED(TerrainBindings);

SGD_SHARED(Terrain)

struct Terrain : Entity {
	SGD_OBJECT_TYPE(Terrain, Entity);

	Terrain();

	TerrainBindings* bindings()const {
		return m_bindings;
	}

private:
	TerrainBindingsPtr m_bindings;

	explicit Terrain(const Terrain* that);

	Terrain* onCopy() const override;

	void onHide() override;
	void onShow() override;
	void init();
};

}
