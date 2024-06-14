#pragma once

#include "bindgroup.h"
#include "shaders/uniforms.h"

// Should maybe be Environment?

namespace sgd {

SGD_SHARED(SceneBindings);

extern const BindGroupDescriptor sceneBindingsDescriptor;

struct SceneBindings : GraphicsResource {
	SGD_OBJECT_TYPE(SceneBindings, GraphicsResource);

	struct ShadowPass {
		TexturePtr renderTarget;
		BindGroupPtr sceneBindings;
	};

	SceneBindings();

	Property<CTexturePtr> envTexture;

	Property<uint32_t> csmTextureSize{1024};
	Property<uint32_t> maxCSMLights{1};
	Property<Array<float,4>> csmSplits{{12,32,64,128}};

	Property<uint32_t> psmTextureSize{1024};
	Property<uint32_t> maxPSMLights{8};
	Property<float> psmNear{{.1f}};

	void setCameraUniforms(CCameraUniforms uniforms);
	CCameraUniforms cameraUniforms() const {
		return *(const CameraUniforms*)m_cameraUniforms->data();
	}

	void setLightingUniforms(CLightingUniforms uniforms);
	CLightingUniforms lightingUniforms() const {
		return *(const LightingUniforms*)m_lightingUniforms->data();
	}

	CVector<ShadowPass> shadowPasses() const {
		return m_shadowPasses;
	}

	BindGroup* bindGroup() const {
		return m_bindGroup;
	}

private:
	BindGroupPtr m_bindGroup;
	BufferPtr m_cameraUniforms;
	BufferPtr m_lightingUniforms;
	BufferPtr m_shadowUniforms;

	// Config
	mutable TexturePtr m_csmTexture;
	mutable Vector<ShadowPass> m_csmShadowPasses;

	mutable TexturePtr m_psmTexture;
	mutable Vector<ShadowPass> m_psmShadowPasses;

	// Passes
	mutable Vector<ShadowPass> m_shadowPasses;

	mutable bool m_configDirty{};
	mutable bool m_passesDirty{};

	static BindGroup* createShadowPassBindings();

	void updateCSMConfig() const;
	void updatePSMConfig() const;

	void addCSMPasses() const;
	void addPSMPasses() const;

	void onValidate(GraphicsContext* gc) const override;
};

} // namespace sgd
