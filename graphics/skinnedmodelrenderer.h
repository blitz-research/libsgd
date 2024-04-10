#pragma once

#include "skinnedmeshrenderer.h"

namespace sgd {

SGD_SHARED(Model);
SGD_SHARED(SkinnedModelRenderer);

struct SkinnedModelRenderer : Renderer {

	SkinnedModelRenderer();

	void add(CModel* model);

	void remove(CModel* model);

private:
	struct InstanceList {
		SkinnedMeshRendererPtr meshRenderer;
		Vector<CModelPtr> models;
	};
	Map<CMesh*, std::unique_ptr<InstanceList>> m_instanceLists;

	void onUpdate(CVec3r eye) override;

	void render(RenderContext* rc) const override;
};

}
