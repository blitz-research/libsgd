#pragma once

#include "entity.h"
#include "mesh.h"

namespace sgd {

SGD_SHARED(Model);

struct Model : Entity {
	SGD_OBJECT_TYPE(Model, Entity);

	Property<CMeshPtr> mesh;

	Property<Vec4f> color{Vec4f{1, 1, 1, 1}};

private:

	void onCreate() override;
	void onShow() override;
	void onHide() override;
};

} // namespace sgd
