#pragma once

#include "entity.h"
#include "mesh.h"

namespace sgd {

SGD_SHARED(ModelRenderer);
SGD_SHARED(Model);

struct Model : Entity {
	SGD_OBJECT_TYPE(Model, Entity);

	Model(CMesh* mesh=nullptr);

	Property<CMeshPtr> mesh;
	Property<Vec4f> color{Vec4f{1, 1, 1, 1}};

private:
	ModelRendererPtr m_renderer;

	void onShow() override;
	void onHide() override;
};

} // namespace sgd
