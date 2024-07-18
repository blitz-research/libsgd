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
		add(model);
	});
}

void ModelRenderer::remove(CModel* model) {
	auto mesh = model->mesh();
	auto it = m_instanceLists.find(mesh);
	SGD_ASSERT(it != m_instanceLists.end());
	sgd::remove(it->second->models, model);
	model->mesh.changed.disconnect(this);
}

void ModelRenderer::update(CVec3r eye) {
	for (auto& kv : m_instanceLists) {
		CMesh* mesh = kv.first;
		if (!mesh) continue;
		auto list = kv.second.get();
		auto inst = list->meshRenderer->lockInstances(list->models.size());
		for (CModel* model : list->models) {
			inst->worldMatrix = model->worldMatrix();
			inst->worldMatrix.t.xyz() -= eye;
			inst->color = model->color();
			++inst;
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
