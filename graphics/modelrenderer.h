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
	Map<CMesh*, UniquePtr<InstanceList>> m_instanceLists;

	void onUpdate(CVec3r eye) override;

	void render(RenderContext* rc) const override;
};

}
