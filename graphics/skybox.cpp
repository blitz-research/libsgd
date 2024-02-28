#include "skybox.h"

#include "scene.h"

namespace sgd {

Skybox::Skybox() {
	skyTexture.changed.connect(this, [=](Texture* texture) { //
		if (m_renderer) m_renderer->skyTexture = texture;
	});
}

void Skybox::onCreate() {
	SGD_ASSERT(!scene()->getRenderer(RendererType::skybox));

	m_renderer = new SkyboxRenderer();
	scene()->setRenderer(RendererType::skybox, m_renderer);

	m_renderer->skyTexture = skyTexture();
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
