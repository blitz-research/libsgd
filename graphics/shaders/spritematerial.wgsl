R"(

// ***** PBRMaterial *****

struct SpriteMaterialUniforms {
    albedoColor: vec4f,
}

@group(1) @binding(0) var<uniform> material_uniforms: SpriteMaterialUniforms;
@group(1) @binding(1) var material_albedoTexture: texture_2d<f32>;
@group(1) @binding(2) var material_albedoSampler: sampler;

fn evaluateMaterial(position: vec3f, normal: vec3f, texCoords: vec2f, color: vec4f) -> MaterialResult {

    var result: MaterialResult;

	result.albedo = textureSample(material_albedoTexture, material_albedoSampler, texCoords) * material_uniforms.albedoColor * color;

	return result;
}

)"
