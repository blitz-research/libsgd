#include "skinnedmodelrenderer.h"

#include "model.h"

namespace sgd {

SkinnedModelRenderer::SkinnedModelRenderer() {
	m_instanceLists.insert(std::make_pair(nullptr, new InstanceList{}));
}

void SkinnedModelRenderer::add(CModel* model) {
	auto mesh = model->mesh();
	auto it = m_instanceLists.find(mesh);
	if (it == m_instanceLists.end()) {
		auto renderer = new SkinnedMeshRenderer(mesh);
		it = m_instanceLists.insert(std::make_pair(mesh, new InstanceList{renderer})).first;
	}
	auto list = it->second.get();
	list->models.emplace_back(model);
	model->mesh.changed.connect(this, [=](CMesh*) {
		sgd::remove(list->models, model);
		add(model);
	});
}

void SkinnedModelRenderer::remove(CModel* model) {
	auto mesh = model->mesh();
	auto it = m_instanceLists.find(mesh);
	SGD_ASSERT(it != m_instanceLists.end());
	sgd::remove(it->second->models, model);
	model->mesh.changed.disconnect(this);
}

void SkinnedModelRenderer::update(CVec3r eye) {
	for (auto& kv : m_instanceLists) {
		CMesh* mesh = kv.first;
		if (!mesh) continue;
		auto list = kv.second.get();
		auto inst = list->meshRenderer->lockInstances(list->models.size());
		for (CModel* model : list->models) {
			auto& worldMatrix = model->worldMatrix();
			inst->matrix.i = {worldMatrix.r.i, 0};
			inst->matrix.j = {worldMatrix.r.j, 0};
			inst->matrix.k = {worldMatrix.r.k, 0};
			inst->matrix.t = {worldMatrix.t - eye, 1};
			inst->color = model->color();
			auto mp = inst->jointMatrices;
			for (auto& matrix : model->jointMatrices()) {
				mp->i = {matrix.r.i, 0};
				mp->j = {matrix.r.j, 0};
				mp->k = {matrix.r.k, 0};
				mp->t = {matrix.t - eye, 1};
				++mp;
			}
			++inst;
		}
		list->meshRenderer->unlockInstances();
	}
}

void SkinnedModelRenderer::render(RenderQueue* rq) const {
	for (auto& kv : m_instanceLists) {
		if (kv.first) kv.second->meshRenderer->render(rq);
	}
}

} // namespace sgd
