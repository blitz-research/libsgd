#include "skyboxbindgroup.h"

namespace sgd {

SGD_SHARED(Material);
SGD_SHARED(RenderQueue);

SGD_SHARED(SkyboxBindings);

struct SkyboxBindings : GraphicsResource {
	SGD_OBJECT_TYPE(SkyboxBindings, GraphicsResource);

	SkyboxBindings();

	Property<CTexturePtr> skyTexture{};

	Property<AffineMat4f> worldMatrix{};

	Property<float> roughness{};

	void render(RenderQueue* rq) const;

private:
	MaterialPtr m_material;
	BindGroupPtr m_bindGroup;
	BufferPtr m_uniforms;

	const SkyboxUniforms& uniforms() const {
		return *(SkyboxUniforms*)m_uniforms->lock(0, sizeof(SkyboxUniforms));
	}
	SkyboxUniforms& lockUniforms();
	void unlockUniforms();
};

} // namespace sgd
