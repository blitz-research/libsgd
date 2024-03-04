#pragma once

#include "entity.h"

#include "skyboxrenderer.h"

namespace sgd {

SGD_SHARED(Skybox);

struct Skybox : Entity {
	SGD_OBJECT_TYPE(Skybox, Entity);

	Skybox();

	explicit Skybox(const Skybox* that);

	Property<TexturePtr> skyTexture;

	Property<float> roughness;

private:
	SkyboxRendererPtr m_renderer;

	void onCreate() override;
	void onDestroy() override;
	void onShow() override;
	void onHide() override;

	void init();
	Skybox* onCopy() const override;
};

}

