#include "skybox.h"

#include "scene.h"

namespace sgd {

void Skybox::init() {
	skyTexture.changed.connect(this, [=](Texture* texture) { //
		if (m_renderer) m_renderer->skyTexture = texture;
	});
	roughness.changed.connect(this, [=](float roughness) { //
		if (m_renderer) m_renderer->roughness = roughness;
	});
}

Skybox::Skybox() {
	init();
}

Skybox::Skybox(const Skybox* that) : Entity(that), //
	skyTexture(that->skyTexture()),
	roughness(that->roughness()) {
	init();
}

Skybox* Skybox::onCopy() const {
	return new Skybox(this);
}

void Skybox::onCreate() {
	SGD_ASSERT(!scene()->getRenderer(RendererType::skybox));

	m_renderer = new SkyboxRenderer();
	m_renderer->skyTexture = skyTexture();
	m_renderer->roughness = roughness();
	m_renderer->enabled = false;

	scene()->setRenderer(RendererType::skybox, m_renderer);
}

void Skybox::onDestroy() {
	scene()->setRenderer(RendererType::skybox, nullptr);
}

void Skybox::onShow() {
	m_renderer->enabled = true;
}

void Skybox::onHide() {
	m_renderer->enabled = false;
}

} // namespace sgd
