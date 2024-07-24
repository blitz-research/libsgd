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
	m_geometry = new SkyboxGeometry();
	m_geometry->skyTexture = skyTexture();
	skyTexture.changed.connect(nullptr, [=](Texture* texture) { //
		m_geometry->skyTexture = texture;
	});
	m_geometry->roughness = roughness();
	roughness.changed.connect(nullptr, [=](float rroughness) { //
		m_geometry->roughness = rroughness;
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
