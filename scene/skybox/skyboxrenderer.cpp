#include "skyboxrenderer.h"

namespace sgd {

void SkyboxRenderer::add(CSkybox* skybox) {
	m_skyboxes.emplace_back(skybox);
}

void SkyboxRenderer::remove(CSkybox* skybox) {
	sgd::remove(m_skyboxes, skybox);
}

void SkyboxRenderer::update(CVec3r eye) {
	for (CSkybox* skybox : m_skyboxes) {
		auto bindings = skybox->skyboxBindings();
		bindings->worldMatrix = skybox->worldMatrix();
	}
}

void SkyboxRenderer::render(RenderQueue* rq) const {
	for (CSkybox* skybox : m_skyboxes) {
		auto bindings = skybox->skyboxBindings();
		bindings->render(rq);
	}
}

} // namespace sgd
