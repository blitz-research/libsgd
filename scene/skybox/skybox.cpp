#include "skybox.h"

#include "../scene/scenerenderer.h"
#include "skyboxrenderer.h"

#include <graphics/exports.h>

namespace sgd {

Skybox::Skybox() {
	init();
}

Skybox::Skybox(Texture* texture) : skyTexture(texture) {
	init();
}

Skybox::Skybox(const Skybox* that)
	: Entity(that),					  //
	  skyTexture(that->skyTexture()), //
	  roughness(that->roughness()) {  //
	init();
}

void Skybox::init() {
	m_bindings = new SkyboxBindings();

	m_bindings->skyTexture = skyTexture();
	skyTexture.changed.connect(nullptr, [=](CTexture* ctexture) { //
		m_bindings->skyTexture = ctexture;
	});

	m_bindings->roughness = roughness();
	roughness.changed.connect(nullptr, [=](float r) { //
		m_bindings->roughness = r;
	});
}

Skybox* Skybox::onCopy() const {
	return new Skybox(this);
}

void Skybox::onShow() {
	scene()->sceneRenderer()->skyboxRenderer()->add(this);
}

void Skybox::onHide() {
	scene()->sceneRenderer()->skyboxRenderer()->remove(this);
}

} // namespace sgd
