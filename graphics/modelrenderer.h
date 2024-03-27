#pragma once

#include "meshrenderer.h"

namespace sgd {

SGD_SHARED(Model);
SGD_SHARED(ModelRenderer);

struct ModelRenderer : Renderer {

	ModelRenderer();

	void add(CModel* model);

	void remove(CModel* model);

private:
	struct InstanceList {
		MeshRendererPtr meshRenderer;
		Vector<CModelPtr> models;
	};
	Map<CMesh*, InstanceList*> m_instanceLists;

	void onUpdate(CVec3f eye) override;

	void onRender(GraphicsContext* gc) const override;
};

}
