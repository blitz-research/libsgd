#pragma once

#include "entity.h"

namespace sgd {

SGD_SHARED(SkyboxRenderer);
SGD_SHARED(Texture);
SGD_SHARED(Skybox);

struct Skybox : Entity {
	SGD_OBJECT_TYPE(Skybox, Entity);

	Skybox();

	explicit Skybox(const Skybox* that);

	Property<TexturePtr> skyTexture;

	Property<float> roughness;

	Property<Mat3f> matrix;

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
