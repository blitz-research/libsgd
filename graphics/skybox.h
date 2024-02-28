#pragma once

#include "entity.h"

#include "skyboxrenderer.h"

namespace sgd {

SGD_SHARED(Skybox);

struct Skybox : Entity {
	SGD_OBJECT_TYPE(Skybox, Entity);

	Skybox();

	Property<TexturePtr> skyTexture;

private:
	SkyboxRendererPtr m_renderer;

	void onCreate() override;
	void onDestroy() override;
	void onShow() override;
	void onHide() override;
};

}

