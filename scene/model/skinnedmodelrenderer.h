#pragma once

#include "model.h"

#include <graphics/exports.h>

namespace sgd {

SGD_SHARED(SkinnedModelRenderer);

struct SkinnedModelRenderer : Shared {
	SGD_OBJECT_TYPE(SkinnedModelRenderer, Shared);

	SkinnedModelRenderer();

	void add(CModel* model);
	void remove(CModel* model);

	void update(CVec3r eye);
	void render(RenderQueue* rq) const;

private:
	struct InstanceList {
		SkinnedMeshRendererPtr meshRenderer;
		Vector<CModelPtr> models;
	};
	Map<CMesh*, std::unique_ptr<InstanceList>> m_instanceLists;
};

}
