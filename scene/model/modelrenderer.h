#pragma once

#include "model.h"

#include <graphics/exports.h>

namespace sgd {

SGD_SHARED(ModelRenderer);

struct ModelRenderer : Shared {
	SGD_OBJECT_TYPE(ModelRenderer, Shared);

	ModelRenderer();

	void add(CModel* model);
	void remove(CModel* model);

	void update(CVec3r eye);
	void render(RenderQueue* rq) const;

private:
	struct InstanceList {
		MeshRendererPtr meshRenderer;
		Vector<CModelPtr> models;
	};
	Map<CMesh*, UniquePtr<InstanceList>> m_instanceLists;
};

}
