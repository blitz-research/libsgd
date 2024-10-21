R"(

// ***** EmissiveMaterial *****

struct EmissiveMaterialUniforms {
    emissiveColor: vec4f,
}

// Material bindings
@group(1) @binding(0) var<uniform> material_uniforms: EmissiveMaterialUniforms;
@group(1) @binding(1) var material_emissiveTexture: texture_2d<f32>;
@group(1) @binding(2) var material_emissiveSampler: sampler;

fn evaluateMaterial(position: vec3f, tanMatrix: mat3x3f, texCoords: vec3f, color: vec4f) -> vec4f {

	let emissive = textureSample(material_emissiveTexture, material_emissiveSampler, texCoords.xy) * material_uniforms.emissiveColor * color;

#if BLEND_MODE_ALPHA_MASK
	if(emissive.w < 0.5) { discard; }
#endif

	return emissive;
}

)"
