R"(

// ***** PBRMaterial *****

struct MaterialUniforms {
    albedoColor: vec4f,
    emissiveColor: vec3f,
    metallicFactor:f32,
    roughnessFactor:f32,
}

@group(1) @binding(0) var<uniform> material_uniforms: MaterialUniforms;
@group(1) @binding(1) var material_albedoTexture: texture_2d<f32>;
@group(1) @binding(2) var material_albedoSampler: sampler;
@group(1) @binding(3) var material_emissiveTexture: texture_2d<f32>;
@group(1) @binding(4) var material_emissiveSampler: sampler;
@group(1) @binding(5) var material_metallicTexture: texture_2d<f32>;
@group(1) @binding(6) var material_metallicSampler: sampler;
@group(1) @binding(7) var material_roughnessTexture: texture_2d<f32>;
@group(1) @binding(8) var material_roughnessSampler: sampler;
@group(1) @binding(9) var material_occlusionTexture: texture_2d<f32>;
@group(1) @binding(10) var material_occlusionSampler: sampler;
@group(1) @binding(11) var material_normalTexture: texture_2d<f32>;
@group(1) @binding(12) var material_normalSampler: sampler;

fn evaluateMaterial(position: vec3f, normal: vec3f, texCoords: vec2f, color: vec4f) -> MaterialResult {

    var result: MaterialResult;
    
	result.albedo = textureSample(material_albedoTexture, material_albedoSampler, texCoords) * material_uniforms.albedoColor * color;
	result.emissive = textureSample(material_emissiveTexture, material_emissiveSampler, texCoords).rgb * material_uniforms.emissiveColor;
	result.metallic = textureSample(material_metallicTexture, material_metallicSampler, texCoords).b * material_uniforms.metallicFactor;
	result.roughness = textureSample(material_roughnessTexture, material_roughnessSampler, texCoords).g * material_uniforms.roughnessFactor;
	result.occlusion = textureSample(material_occlusionTexture, material_occlusionSampler, texCoords).r;
   	result.normal = textureSample(material_normalTexture, material_normalSampler, texCoords).rgb * 2.0 - 1.0;

	return result;
}

)"
