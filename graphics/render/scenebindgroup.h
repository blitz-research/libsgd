#pragma once

#include "../core/bindgroup.h"
#include "../core/graphics.h"

namespace sgd {

// Note: *MUST* stay in sync with scene.wgsl
constexpr int maxDirectionalLights = 4;
constexpr int maxPointLights = 32;
constexpr int maxSpotLights = 16;

constexpr int configUniformsBinding = 0;
constexpr int cameraUniformsBinding = 1;
constexpr int lightingUniformsBinding = 2;
constexpr int envTextureCubeBinding = 3;
constexpr int envTexture2DBinding = 4;
constexpr int envSamplerBinding = 5;
constexpr int csmTextureBinding = 6;
constexpr int csmSamplerBinding = 7;
constexpr int csmMatricesBinding = 8;
constexpr int psmTextureBinding = 9;
constexpr int psmSamplerBinding = 10;
constexpr int ssmTextureBinding = 11;
constexpr int ssmSamplerBinding = 12;
constexpr int ssmMatricesBinding = 13;

struct alignas(16) ConfigUniforms {

	// CSM settings
	Vec4f csmSplitDistances{Vec4f(15, 60, 250, 1000)};
	uint32_t csmTextureSize{2048};
	uint32_t maxCSMLights{4}; // max = 256/4, ie: max array index.
	float csmClipRange{330.0f};
	float csmDepthBias{.0001f};

	// PSM Settings
	uint32_t psmTextureSize{1024};
	uint32_t maxPSMLights{32}; // max = 256/6
	float psmClipNear{.25f};
	float psmDepthBias{.0001f};

	// SSM Settings
	uint32_t ssmTextureSize{1024};
	uint32_t maxSSMLights{16}; // max = 256
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
	float clipNear{.1};
	float clipFar{1000};
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

	uint32_t envTextureType{1};	// 1=cube, 2=2d

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

ConfigUniforms getConfigUniformsFromConfigVars();

}
