R"(

// ***** PrelitMaterial *****

struct PrelitMaterialUniforms {
    albedoColor: vec4f,
}

@group(1) @binding(0) var<uniform> material_uniforms: PrelitMaterialUniforms;
@group(1) @binding(1) var material_albedoTexture: texture_2d<f32>;
@group(1) @binding(2) var material_albedoSampler: sampler;

fn evaluateMaterial(position: vec3f, tanMatrix: mat3x3f, texCoords: vec2f, color: vec4f) -> vec4f {

	let albedo = textureSample(material_albedoTexture, material_albedoSampler, texCoords) * material_uniforms.albedoColor * color;

	return albedo;
}

)"
