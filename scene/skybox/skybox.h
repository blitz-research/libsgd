#pragma once

#include "../scene/entity.h"

namespace sgd {

SGD_SHARED(SkyboxGeometry);

SGD_SHARED(Texture);
SGD_SHARED(Skybox);

struct Skybox : Entity {
	SGD_OBJECT_TYPE(Skybox, Entity);

	Skybox();
	explicit Skybox(Texture* texture);
	explicit Skybox(const Skybox* that);

	Property<TexturePtr> skyTexture;
	Property<float> roughness;

	SkyboxGeometry* skyboxGeometry() const { return m_geometry;}

private:
	SkyboxGeometryPtr m_geometry;

	Skybox* onCopy() const override;

	void onShow() override;
	void onHide() override;
	void init();
};

}
