#pragma once

#include "../scene/entity.h"

namespace sgd {

SGD_SHARED(SkyboxBindings);

SGD_SHARED(Texture);
SGD_SHARED(Skybox);

struct Skybox : Entity {
	SGD_OBJECT_TYPE(Skybox, Entity);

	Skybox();
	explicit Skybox(Texture* texture);
	explicit Skybox(const Skybox* that);

	Property<TexturePtr> skyTexture{};
	Property<float> roughness{0};

	SkyboxBindings* skyboxBindings() const { return m_bindings;}

private:
	SkyboxBindingsPtr m_bindings;

	Skybox* onCopy() const override;

	void onShow() override;
	void onHide() override;
	void init();
};

}
