#include "modelrenderer.h"

#include "model.h"

namespace sgd {
ModelRenderer::ModelRenderer() {
	m_instanceLists[nullptr] = new InstanceList {nullptr};
}

void ModelRenderer::add(CModel* model) {
	auto mesh=model->mesh();
	auto it = m_instanceLists.find(mesh);
	if(it == m_instanceLists.end()) {
		auto renderer = new MeshRenderer(mesh);
		it = m_instanceLists.insert(std::make_pair(mesh, new InstanceList{renderer})).first;
	}
	auto list=it->second;
	list->models.push_back(model);
	model->mesh.changed.connect(this,[=](CMesh*) {
		sgd::remove(list->models,model);
		add(model);
	});
}

void ModelRenderer::remove(CModel* model) {
	auto mesh=model->mesh();
	auto it = m_instanceLists.find(mesh);
	SGD_ASSERT(it!=m_instanceLists.end());
	sgd::remove(it->second->models, model);
	model->mesh.changed.disconnect(this);
}

void ModelRenderer::onUpdate(CVec3f eye) {
	for(auto [mesh, list] : m_instanceLists) {
		if(!mesh) continue;
		auto inst = list->meshRenderer->lockInstances(list->models.size());
		for(auto model : list->models) {
			inst->matrix = Mat4f(model->worldMatrix());
			inst->color = model->color();
			++inst;
		}
		list->meshRenderer->unlockInstances();
		list->meshRenderer->onUpdate(eye);
	}
}

void ModelRenderer::onRender(GraphicsContext* gc) const {
	for(auto [mesh, list] : m_instanceLists) {
		if(mesh) list->meshRenderer->render(gc);
	}
}

}
