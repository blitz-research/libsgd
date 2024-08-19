#pragma once

#include "scenebindgroup.h"

namespace sgd {

SGD_SHARED(SceneBindings);

struct SceneBindings : GraphicsResource {
	SGD_OBJECT_TYPE(SceneBindings, GraphicsResource);

	struct ShadowPass {
		TexturePtr renderTarget;
		BindGroupPtr sceneBindings;
	};

	SceneBindings();

	CConfigUniforms configUniforms() const {
		return *(ConfigUniforms*)m_configUniforms->data();
	}
	ConfigUniforms& lockConfigUniforms();
	void unlockConfigUniforms();

	CCameraUniforms cameraUniforms() const {
		return *(CameraUniforms*)m_cameraUniforms->data();
	}
	CameraUniforms& lockCameraUniforms();
	void unlockCameraUniforms();

	CLightingUniforms lightingUniforms() const {
		return *(LightingUniforms*)m_lightingUniforms->data();
	}
	LightingUniforms& lockLightingUniforms();
	void unlockLightingUniforms();

	Property<CTexturePtr> envTexture;

	CVector<ShadowPass> shadowPasses() const {
		return m_shadowPasses;
	}

	BindGroup* bindGroup() const {
		return m_bindGroup;
	}

private:
	enum struct Dirty {
		shadowConfigs = 1,
		shadowPasses = 2,
	};

	BindGroupPtr m_bindGroup;
	BufferPtr m_configUniforms;
	BufferPtr m_sceneUniforms;
	BufferPtr m_cameraUniforms;
	BufferPtr m_lightingUniforms;

	mutable TexturePtr m_csmTexture;
	mutable TexturePtr m_psmTexture;
	mutable TexturePtr m_ssmTexture;

	mutable Vector<ShadowPass> m_csmShadowPasses;
	mutable Vector<ShadowPass> m_psmShadowPasses;
	mutable Vector<ShadowPass> m_ssmShadowPasses;
	mutable Vector<ShadowPass> m_shadowPasses;

	mutable Dirty m_dirty{Dirty::shadowConfigs | Dirty::shadowPasses};

	BindGroup* createShadowPassBindings() const;

	void updateCSMConfig() const;
	void addCSMPasses() const;

	void updatePSMConfig() const;
	void addPSMPasses() const;

	void updateSSMConfig() const;
	void addSSMPasses() const;

	void onValidate() const override;
};

} // namespace sgd
