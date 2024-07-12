#pragma once

#include "skybox.h"

#include <graphics/exports.h>

namespace sgd {

SGD_SHARED(SkyboxRenderer);

struct SkyboxRenderer : Shared {
	SGD_OBJECT_TYPE(SkyboxRenderer, Shared);

	void add(CSkybox* skybox);
	void remove(CSkybox* skybox);

	void update(CVec3r eye);
	void render(RenderQueue* rq) const;

private:
	Vector<CSkyboxPtr> m_skyboxes;
};

}
