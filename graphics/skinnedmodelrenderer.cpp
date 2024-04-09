#include "skinnedmodelrenderer.h"

#include "model.h"

#include "shaders/uniforms.h"

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

void SkinnedModelRenderer::onUpdate(CVec3f eye) {
	for(auto& kv : m_instanceLists) {
		CMesh* mesh = kv.first;
		if(!mesh) continue;
		auto list = kv.second.get();
		auto inst = list->meshRenderer->lockInstances(list->models.size());
		for (CModel* model : list->models) {
			inst->worldMatrix = Mat4f(model->worldMatrix());
			inst->color = model->color();
			int i = 0;
			for (auto& matrix : model->jointMatrices()) {
				inst->jointMatrices[i++] = Mat4f(matrix);
			}
			++inst;
		}
		list->meshRenderer->unlockInstances();
		list->meshRenderer->onUpdate(eye);
	}
}

void SkinnedModelRenderer::render(RenderContext* rc) const {
	for(auto& kv : m_instanceLists) {
		if (kv.first) kv.second->meshRenderer->render(rc);
	}
}

} // namespace sgd
