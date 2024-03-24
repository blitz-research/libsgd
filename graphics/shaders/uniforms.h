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

	struct alignas(16) DirectionalLight {
		alignas(16) Vec3f direction{0, 0, -1}; // -forward
		alignas(16) Vec4f color{1};
	};

	struct alignas(16) PointLight {
		alignas(16) Vec3f position{0, 0, 0};
		alignas(16) Vec4f color{1};
		float range{100};
		float falloff{1};
	};

	struct alignas(16) SpotLight {
		alignas(16) Vec3f position{0, 0, 0};
		alignas(16) Vec3f direction{0, 0, 0};
		alignas(16) Vec4f color{1};
		float range{100};
		float falloff{1};
		float innerConeAngle{0};
		float outerConeAngle{halfPi}; // 45 degrees
	};

	static constexpr int maxDirectionalLights = 4;
	static constexpr int maxPointLights = 8;
	static constexpr int maxSpotLights = 8;

	Vec4f ambientLightColor{1, 1, 1, .1};

	uint32_t directionalLightCount{};
	DirectionalLight directionalLights[maxDirectionalLights];

	uint32_t pointLightCount{};
	PointLight pointLights[maxPointLights];

	uint32_t spotLightCount{};
	SpotLight spotLights[maxSpotLights];
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
	Vec4f albedoColor{1, 1, 1, 1};
	Vec3f emissiveColor{0, 0, 0};
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

// ***** SkinnedMeshRenderer *****

//@group(2) @binding(1) var<storage> skinnedMeshInstances: array<SkinnedMeshInstance>;
struct alignas(16) SkinnedMeshInstance {
	static constexpr int maxJoints = 256;
	Mat4f worldMatrix;
	Vec4f color;
	Mat4f jointMatrices[maxJoints];
};

// ***** Sprite Renderer *****

// @group(2) @binding(0) var<storage> spriteInstances: array<SpriteInstance>;
struct SpriteInstance {
	Mat4f matrix;
	Vec4f color;
	Rectf rect;
};

} // namespace sgd
