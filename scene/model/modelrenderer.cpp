#include "modelrenderer.h"

#include "model.h"

namespace sgd {

ModelRenderer::ModelRenderer() {
	m_instanceLists.insert(std::make_pair(nullptr, new InstanceList{}));
}

void ModelRenderer::add(CModel* model) {
	CMesh* mesh = model->mesh();
	auto it = m_instanceLists.find(mesh);
	if (it == m_instanceLists.end()) {
		auto renderer = new MeshRenderer(mesh);
		it = m_instanceLists.insert(std::make_pair(mesh, new InstanceList{renderer})).first;
	}
	auto list = it->second.get();
	list->models.emplace_back(model);
	model->mesh.changed.connect(this, [=](CMesh*) {
		sgd::remove(list->models, model);
		model->mesh.changed.disconnect(this);
		add(model);
	});
}

void ModelRenderer::remove(CModel* model) {
	auto it = m_instanceLists.find(model->mesh());
	SGD_ASSERT(it != m_instanceLists.end());
	auto list = it->second.get();
	model->mesh.changed.disconnect(this);
	sgd::remove(list->models, model);
}

void ModelRenderer::update(CVec3r eye) {
	for (auto& kv : m_instanceLists) {
		if (!kv.first) continue;
		auto list = kv.second.get();
		auto instp = list->meshRenderer->lockInstances(list->models.size());
		for (CModel* model : list->models) {
			instp->worldMatrix = model->worldMatrix();
			instp->worldMatrix.t.xyz() -= eye;
			instp->color = model->color();
			++instp;
		}
		list->meshRenderer->unlockInstances();
	}
}

void ModelRenderer::render(RenderQueue* rq) const {
	for (auto& kv : m_instanceLists) {
		if (kv.first) kv.second->meshRenderer->render(rq);
	}
}

} // namespace sgd
