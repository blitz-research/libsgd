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
		model->mesh.changed.disconnect(this);
		sgd::remove(list->models, model);
		add(model);
	});
}

void SkinnedModelRenderer::remove(CModel* model) {
	auto mesh = model->mesh();
	auto it = m_instanceLists.find(mesh);
	SGD_ASSERT(it != m_instanceLists.end());
	auto list = it->second.get();
	model->mesh.changed.disconnect(this);
	sgd::remove(list->models, model);
}

void SkinnedModelRenderer::update(CVec3r eye) {
	for (auto& kv : m_instanceLists) {
		CMesh* mesh = kv.first;
		if (!mesh) continue;
		auto list = kv.second.get();
		auto instp = list->meshRenderer->lockInstances(list->models.size());
		for (CModel* model : list->models) {
			instp->worldMatrix = model->worldMatrix();
			instp->worldMatrix.t.xyz() -= eye;
			instp->color = model->color();
			auto mp = instp->jointMatrices;
			for (auto& matrix : model->jointMatrices()) {
				(*mp) = matrix;
				(*mp).t.xyz() -= eye;
				++mp;
			}
			++instp;
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
