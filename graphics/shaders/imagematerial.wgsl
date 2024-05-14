R"(

// ***** ImageMaterial *****

struct ImageMaterialUniforms {
    rect: vec4f,
    spriteViewMode: u32,
}

@group(1) @binding(0) var<uniform> material_uniforms: ImageMaterialUniforms;
@group(1) @binding(1) var material_albedoTexture: texture_2d_array<f32>;
@group(1) @binding(2) var material_albedoSampler: sampler;

fn evaluateMaterial(position: vec3f, tanMatrix: mat3x3f, texCoords: vec3f, color: vec4f) -> vec4f {

	let albedo = textureSample(material_albedoTexture, material_albedoSampler, texCoords.xy, u32(texCoords.z)) * color;

    return albedo;
}

)"
