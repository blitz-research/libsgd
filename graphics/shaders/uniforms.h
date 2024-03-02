#pragma once

#include <geom/exports.h>

namespace sgd {

// @group(0) @binding(0) var<uniform> camera_uniforms: CameraUniforms;
struct alignas(16) CameraUniforms {
	Mat4f projectionMatrix;
	Mat4f inverseProjectionMatrix;
	Mat4f viewProjectionMatrix;
	Mat4f worldMatrix;
	Mat4f viewMatrix;
	float clipNear{.1};
	float clipFar{100};
};

// @group(0) @binding(1) var<uniform> lighting_uniforms: LightingUniforms;
// @group(0) @binding(2) var lighting_envTexture: texture_cube<f32>;
// @group(0) @binding(3) var lighting_envSampler: sampler;
struct alignas(16) LightingUniforms {

	static constexpr int maxPointLights = 16;
	static constexpr int maxDirectionalLights = 4;

	struct alignas(16) PointLight {
		Vec4f position{0, 0, 0, 1};
		Vec4f color{1};
		float range{100};
		float falloff{1};
	};

	struct alignas(16) DirectionalLight {
		Vec4f direction{0, 0, 1, 0};
		Vec4f color{1};
	};

	Vec4f ambientLightColor{1, 1, 1, .1};

	uint32_t pointLightCount{0};
	PointLight pointLights[maxPointLights];

	uint32_t directionalLightCount{};
	DirectionalLight directionalLights[maxDirectionalLights];
};

// @group(1) @binding(0) var<uniform> material_uniforms: MaterialUniforms;
// @group(1) @binding(1) var material_albedoTexture: texture_2d<f32>;
// @group(1) @binding(2) var material_albedoSampler: sampler;
struct alignas(16) MatteMaterialUniforms {
	Vec4f albedoColor;
};

// @group(1) @binding(0) var<uniform> material_uniforms: MaterialUniforms;
// @group(1) @binding(1) var material_albedoTexture: texture_2d<f32>;
// @group(1) @binding(2) var material_albedoSampler: sampler;
// @group(1) @binding(3) var material_emissiveTexture: texture_2d<f32>;
// @group(1) @binding(4) var material_emissiveSampler: sampler;
// @group(1) @binding(5) var material_metallicTexture: texture_2d<f32>;
// @group(1) @binding(6) var material_metallicSampler: sampler;
// @group(1) @binding(7) var material_roughnessTexture: texture_2d<f32>;
// @group(1) @binding(8) var material_roughnessSampler: sampler;
// @group(1) @binding(9) var material_occlusionTexture: texture_2d<f32>;
// @group(1) @binding(10) var material_occlusionSampler: sampler;
// @group(1) @binding(11) var material_normalTexture: texture_2d<f32>;
// @group(1) @binding(12) var material_normalSampler: sampler;
struct alignas(16) PBRMaterialUniforms {
	Vec4f albedoColor{1,1,1,1};
	Vec3f emissiveColor{0,0,0};
	float metallicFactor{0};
	float roughnessFactor{1};
};

// @group(2) @binding(0) var<uniform> skybox_uniforms: SkyboxUniforms;
// @group(2) @binding(1) var skybox_skyboxTexture: texture_cube<f32>;
// @group(2) @binding(2) var skybox_skyboxSampler: sampler;
struct alignas(16) SkyboxUniforms {
	float mipmapBias{0};
};

// ***** Mesh renderer *****

// @group(2) @binding(0) var<uniform> meshUniforms: MeshUniforms;
struct alignas(16) MeshUniforms {
	int32_t tangentsEnabled{0};
};

// @location(8) matrix_0: vec4f,
// @location(9) matrix_1: vec4f,
// @location(10) matrix_2: vec4f,
// @location(11) matrix_3: vec4f,
// @location(12) color: vec4f,
struct ModelInstance {
	Mat4f matrix;
	Vec4f color;
};

} // namespace sgd
