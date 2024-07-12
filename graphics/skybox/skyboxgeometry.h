#pragma once

#include "../core/bindgroup.h"

namespace sgd {

SGD_SHARED(Material);
SGD_SHARED(RenderQueue);

SGD_SHARED(SkyboxGeometry);

struct SkyboxGeometry : GraphicsResource {
	SGD_OBJECT_TYPE(SkyboxGeometry, GraphicsResource);

	SkyboxGeometry();

	Property<TexturePtr> skyTexture;
	Property<AffineMat4r> worldMatrix;
	Property<float> roughness;

	void render(RenderQueue* rq) const;

private:
	MaterialPtr m_material;
	BufferPtr m_uniformBuffer;
	BindGroupPtr m_bindGroup;

	void onValidate() const override;
};

} // namespace sgd
