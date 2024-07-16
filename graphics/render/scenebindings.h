#pragma once

#include "../core/bindgroup.h"
#include "../core/graphics.h"

namespace sgd {

// Note: Must sync with scene.wgsl

constexpr int maxDirectionalLights = 8;
constexpr int maxPointLights = 32;
constexpr int maxSpotLights = 64;

struct alignas(16) ConfigUniforms {

	// CSM settings
	Vec4f csmSplitDistances{Vec4f(16, 64, 256, 1024)};
	uint32_t csmTextureSize{2048};
	uint32_t maxCSMLights{4}; // max = 256/4, ie: max array index.
	float csmClipRange{330.0f};
	float csmDepthBias{.0001f};

	// PSM Settings
	uint32_t psmTextureSize{1024};
	uint32_t maxPSMLights{8}; // max = 256/6
	float psmClipNear{.25f};
	float psmDepthBias{.0001f};

	// SSM Settings
	uint32_t ssmTextureSize{1024};
	uint32_t maxSSMLights{4}; // max = 256
	float ssmClipNear{.25f};
	float ssmDepthBias{.0001f};
};
using CConfigUniforms = const ConfigUniforms&;

struct alignas(16) CameraUniforms {
	Mat4f projectionMatrix;
	Mat4f inverseProjectionMatrix;
	Mat4f viewProjectionMatrix;
	Mat4f worldMatrix;
	Mat4f viewMatrix;
	float clipNear{.125};
	float clipFar{1024};
};
using CCameraUniforms = const CameraUniforms&;

struct alignas(16) DirectionalLightUniforms {
	Mat4f worldMatrix;
	Vec4f color{1};
};

struct alignas(16) PointLightUniforms {
	Vec4f position;
	Vec4f color;
	float range{100};
	float falloff{1};
};

struct alignas(16) SpotLightUniforms {
	Mat4f worldMatrix;
	Vec4f color{1};
	float range{100};
	float falloff{1};
	float innerConeAngle{0};
	float outerConeAngle{halfPi}; // 45 degrees
};

struct alignas(16) LightingUniforms {
	Vec4f ambientLightColor;

	uint32_t numDirectionalLights{};
	uint32_t numPointLights{};
	uint32_t numSpotLights{};

	uint32_t numCSMLights{};
	uint32_t numPSMLights{};
	uint32_t numSSMLights{};

	DirectionalLightUniforms directionalLights[maxDirectionalLights];
	PointLightUniforms pointLights[maxPointLights];
	SpotLightUniforms spotLights[maxSpotLights];
};
using CLightingUniforms = const LightingUniforms&;

extern const BindGroupDescriptor sceneBindingsDescriptor;
extern const BindGroupDescriptor sceneShadowBindingsDescriptor;

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
