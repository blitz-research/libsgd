R"(

// ***** ImageMaterial *****

struct ImageMaterialUniforms {
    albedoColor: vec4f,
}

// Material bindings
@group(1) @binding(0) var<uniform> material_uniforms: ImageMaterialUniforms;
#if IMAGE_MATERIAL_MX
@group(1) @binding(1) var material_albedoTexture: texture_2d_array<f32>;
#else
@group(1) @binding(1) var material_albedoTexture: texture_2d<f32>;
#endif
@group(1) @binding(2) var material_albedoSampler: sampler;

fn evaluateMaterial(position: vec3f, tanMatrix: mat3x3f, texCoords: vec3f, color: vec4f) -> vec4f {

#if IMAGE_MATERIAL_MX
	let albedo = textureSample(material_albedoTexture, material_albedoSampler, texCoords.xy, u32(texCoords.z)) * material_uniforms.albedoColor * color;
#else
	let albedo = textureSample(material_albedoTexture, material_albedoSampler, texCoords.xy) * material_uniforms.albedoColor * color;
#endif

#if BLEND_MODE_ALPHA_MASK
	if(albedo.a < 0.5) { discard; }
#endif

    return albedo;
}

)"
