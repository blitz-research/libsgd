#pragma once

#include <geom/exports.h>

namespace sgd {

// ***** Scene *****

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
	float csmDepthBias{0.0001f};
	float psmClipNear{0.1f};
	float psmDepthBias{0.0001f};
};
using CShadowUniforms = const ShadowUniforms&;

// @group(0) @binding(0) var<uniform> cameraUniforms: CameraUniforms;
// @group(0) @binding(1) var<uniform> lightingUniforms: LightingUniforms;
// @group(0) @binding(2) var<uniform> shadowUniforms: ShadowUniforms;
// @group(0) @binding(3) var lighting_envTexture: texture_cube<f32>;
// @group(0) @binding(4) var lighting_envSampler: sampler;

// @group(0) @binding(5) var lighting_csmTexture: texture_depth_2d_array;
// @group(0) @binding(6) var lighting_csmSampler: sampler;
// @group(0) @binding(7) var lighting_csmMatrices: array<mat4x4f>;

// @group(0) @binding(8) var lighting_psmTexture: texture_depth_cube_array;
// @group(0) @binding(9) var lighting_psmSampler: sampler;

// ***** MatteMaterial *****

struct alignas(16) MatteMaterialUniforms {
	Vec4f albedoColor{1};
	float metallicFactor{0};
	float roughnessFactor{1};
};
// @group(1) @binding(0) var<uniform> material_uniforms: MaterialUniforms;
// @group(1) @binding(1) var material_albedoTexture: texture_2d<f32>;
// @group(1) @binding(2) var material_albedoSampler: sampler;

// ***** PBRMaterial *****

struct alignas(16) PBRMaterialUniforms {
	Vec4f albedoColor{1};
	Vec3f emissiveColor{0};
	float metallicFactor{0};
	float roughnessFactor{1};
};
// @group(1) @binding(0) var<uniform> material_uniforms: PBRMaterialUniforms;
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

// ***** SkyboxRenderer *****

struct alignas(16) SkyboxUniforms {
	Mat4f worldMatrix;
	float mipmapBias{0};
};
// @group(2) @binding(0) var<uniform> skybox_uniforms: SkyboxUniforms;
// @group(2) @binding(1) var skybox_skyboxTexture: texture_cube<f32>;
// @group(2) @binding(2) var skybox_skyboxSampler: sampler;

// ***** MeshRenderer *****

struct alignas(16) MeshInstance {
	Mat4f matrix;
	Vec4f color;
};

struct alignas(16) MeshUniforms {
	int32_t tangentsEnabled{0};
};
// @group(2) @binding(0) var<uniform> meshUniforms: MeshUniforms;

// ***** SkinnedMeshRenderer *****

struct alignas(16) SkinnedMeshInstance {
	static constexpr int maxJoints = 256;
	Mat4f matrix;
	Vec4f color;
	Mat4f jointMatrices[maxJoints];
};

struct alignas(16) SkinnedMeshUniforms {
	int32_t tangentsEnabled{0};
};
// @group(2) @binding(0) var<uniform> meshUniforms: SkinnedMeshUniforms;
// @group(2) @binding(1) var<storage> meshInstances: array<SkinnedMeshInstance>;

// ***** PrelitMaterial *****

// @group(1) @binding(0) var<uniform> material_uniforms: PrelitMaterialUniforms;
// @group(1) @binding(1) var material_albedoTexture: texture_2d<f32>;
// @group(1) @binding(2) var material_albedoSampler: sampler;
struct alignas(16) PrelitMaterialUniforms {
	Vec4f albedoColor{1};
};

// ***** FontMaterial *****

// @group(1) @binding(0) var<uniform> material_uniforms: FontMaterialUniforms;
// @group(1) @binding(1) var material_atlasTexture: texture_2d<f32>;
// @group(1) @binding(2) var material_atlasSampler: sampler;
struct alignas(16) FontMaterialUniforms {
	Vec4f atlasColor{1};
};

// ***** SpriteRenderer *****

struct alignas(16) SpriteInstance {
	Mat4f matrix;
	Vec4f color;
	float frame{};
};
// @group(2) @binding(0) var<storage> spriteInstances: array<SpriteInstance>;

// ***** DrawListRenderer *****

struct DrawListUniforms {
	Mat4f matrix;
};

// ***** ImageUniforms *****

struct alignas(16) ImageMaterialUniforms {
	Rectf rect;
	uint32_t spriteViewMode;
};

} // namespace sgd
