#include "model.h"

#include "scene.h"

#include "meshrenderer.h"

namespace sgd {

SGD_SHARED(ModelRenderer);

struct ModelRenderer : Renderer {
	SGD_OBJECT_TYPE(ModelRenderer, Renderer);

	ModelRenderer(CMesh* mesh) : m_meshRenderer(new MeshRenderer(mesh)) {
		addDependency(m_meshRenderer, false);
	}

	void add(CModel* model) {
		m_models.push_back(model);
	}

	void remove(CModel* model) {
		sgd::remove(m_models, model);
	}

private:
	MeshRendererPtr m_meshRenderer;
	Vector<CModelPtr> m_models;

	void onUpdate() const override {
		auto insts = m_meshRenderer->lockInstances(m_models.size());
		for (uint32_t i = 0; i < m_models.size(); ++i) {
			insts[i].matrix = Mat4f(m_models[i]->worldMatrix());
			insts[i].color = m_models[i]->color();
		}
		m_meshRenderer->unlockInstances();
		m_meshRenderer->onUpdate();
	}

	void onValidate(GraphicsContext* gc) const override {
		m_renderPassMask = m_meshRenderer->renderPassMask();
	}

	void onRender(GraphicsContext* gc) const override {
		m_meshRenderer->render(gc);
	}
};

Model::Model(CMesh* cmesh) : mesh(cmesh) {
	mesh.changed.connect(this, [=](CMesh*) {
		onHide();
		if (visible()) onShow();
	});
}

void Model::onShow() {
	log() << "### onShow:"<<m_renderer<<mesh();
	SGD_ASSERT(!m_renderer);
	if (!mesh()) return;
	auto r = scene()->getRenderer(mesh());
	m_renderer = r ? r->as<ModelRenderer>() : nullptr;
	if (!m_renderer) {
		m_renderer = new ModelRenderer(mesh());
		scene()->addRenderer(mesh(), m_renderer);
	}
	m_renderer->add(this);
}

void Model::onHide() {
	log() << "### onHide:"<<m_renderer<<mesh();
	if (!m_renderer) return;
	m_renderer->remove(this);
	m_renderer = nullptr;
}

} // namespace sgd
