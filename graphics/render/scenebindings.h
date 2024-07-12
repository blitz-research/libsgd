#pragma once

#include "../core/graphics.h"
#include "../core/bindgroup.h"

// Should maybe be Environment?

namespace sgd {

struct alignas(16) CameraUniforms {
	Mat4f projectionMatrix;
	Mat4f inverseProjectionMatrix;
	Mat4f viewProjectionMatrix;
	Mat4f worldMatrix;
	Mat4f viewMatrix;
	float clipNear{.1};
	float clipFar{100};
};
using CCameraUniforms = const CameraUniforms&;

struct alignas(16) LightingUniforms {
	static constexpr int maxDirectionalLights = 4;
	static constexpr int maxPointLights = 32;
	static constexpr int maxSpotLights = 16;

	struct alignas(16) DirectionalLight {
		Mat4f worldMatrix;
		Vec4f color{1};
		int shadowsEnabled{0};
	};
	static_assert(sizeof(DirectionalLight) == 96);

	struct alignas(16) PointLight {
		Vec3f position;
		alignas(16) Vec4f color{1};
		float range{100};
		float falloff{1};
		int shadowsEnabled{0};
	};
	static_assert(sizeof(PointLight) == 48);

	struct alignas(16) SpotLight {
		alignas(16) Vec3f position{0, 0, 0};
		alignas(16) Vec3f direction{0, 0, 0};
		alignas(16) Vec4f color{1};
		float range{100};
		float falloff{1};
		float innerConeAngle{0};
		float outerConeAngle{halfPi}; // 45 degrees
	};

	Vec4f ambientLightColor{1, 1, 1, 0};

	uint32_t directionalLightCount{};
	DirectionalLight directionalLights[maxDirectionalLights];

	uint32_t pointLightCount{};
	PointLight pointLights[maxPointLights];

	uint32_t spotLightCount{};
	SpotLight spotLights[maxSpotLights];
};
using CLightingUniforms = const LightingUniforms&;

struct alignas(16) ShadowUniforms {
	Array<float, 4> csmSplitDistances{};
	float csmClipRange{1000.0f};
	float csmDepthBias{0.0001f};
	float psmClipNear{0.1f};
	float psmDepthBias{0.0001f};
};
using CShadowUniforms = const ShadowUniforms&;

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

	Property<uint32_t> csmTextureSize{2048};
	Property<uint32_t> maxCSMLights{1};
	Property<uint32_t> psmTextureSize{1024};
	Property<uint32_t> maxPSMLights{8};

	Property<Array<float, 4>> csmSplitDistances{{16, 64, 256, 1024}};
	Property<float> csmClipRange{1000.0f};
	Property<float> csmDepthBias{.0001f};

	Property<float> psmClipNear{.01f};
	Property<float> psmDepthBias{.0001f};

	void setCameraUniforms(CCameraUniforms uniforms);
	CCameraUniforms cameraUniforms() const {
		return *(const CameraUniforms*)m_cameraUniforms->data();
	}

	void setLightingUniforms(CLightingUniforms uniforms);
	CLightingUniforms lightingUniforms() const {
		return *(const LightingUniforms*)m_lightingUniforms->data();
	}

	void setShadowUniforms(CShadowUniforms uniforms);
	CShadowUniforms shadowUniforms()const {
		return *(const ShadowUniforms*)m_shadowUniforms->data();
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

	mutable bool m_configDirty{true};
	mutable bool m_passesDirty{true};
	mutable bool m_uniformsDirty{true};

	static BindGroup* createShadowPassBindings();

	void updateCSMConfig() const;
	void updatePSMConfig() const;

	void addCSMPasses() const;
	void addPSMPasses() const;

	void onValidate() const override;
};

} // namespace sgd
