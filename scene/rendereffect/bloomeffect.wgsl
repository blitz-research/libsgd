R"(

@group(2) @binding(0) var effect_sourceTexture: texture_2d<f32>;
@group(2) @binding(1) var effect_sourceSampler: sampler;

struct Varying {
    @builtin(position) clipPosition: vec4f,
    @location(0) texCoords: vec2f
}

@vertex fn vertexMain(@builtin(vertex_index) vertexId: u32) -> Varying {

    const vertices = array<vec2f, 4>(vec2f(-1.0, 1.0), vec2f(1.0, 1.0), vec2f(-1.0, -1.0), vec2f(1.0, -1.0));

    let vertex = vertices[vertexId];

    var out : Varying;

    out.clipPosition = vec4f(vertex, 0.0, 1.0);
    out.texCoords = vertex * vec2f(0.5, -0.5) + 0.5;

    return out;
}

@fragment fn fragmentMain(in: Varying) -> @location(0) vec4f {

#if BLOOM_PASS_0

	let color = textureSampleLevel(effect_sourceTexture, effect_sourceSampler, in.texCoords, 0);
	return vec4f(max(color.rgb - vec3(1.0), vec3(0.0)), color.a);

#elif BLOOM_PASS_1

	return textureSampleLevel(effect_sourceTexture, effect_sourceSampler, in.texCoords, 0);

#elif BLOOM_PASS_2 || BLOOM_PASS_3

	const kernelSize = 2u;
	const offsets = array<f32, kernelSize>(0.53805, 2.06278);
	const weights = array<f32, kernelSize>(0.44908, 0.05092);

//	const kernelSize = 9u;
// 	const offsets = array<f32, kernelSize>(0.66293, 2.47904, 4.46232, 6.44568, 8.42917, 10.41281, 12.39664, 14.38070, 16.36501);
// 	const weights = array<f32, kernelSize>(0.10855, 0.13135, 0.10406, 0.07216, 0.04380,  0.02328,  0.01083,  0.00441,  0.00157);

#if BLOOM_PASS_2
	let halfTexelSize = vec2f(1.0 / f32(textureDimensions(effect_sourceTexture).x), 0.0);
#else
	let halfTexelSize = vec2f(0.0, 1.0 / f32(textureDimensions(effect_sourceTexture).y));
#endif

	var color = vec3f(0.0);

	for(var i=0u; i < kernelSize; i+=1) {

		let offset = halfTexelSize * offsets[i];

		color += textureSampleLevel(effect_sourceTexture, effect_sourceSampler, in.texCoords + offset, 0).rgb * weights[i];
		color += textureSampleLevel(effect_sourceTexture, effect_sourceSampler, in.texCoords - offset, 0).rgb * weights[i];
	}

	return vec4f(color, 0.0);   // 0.0 alpha for additive blending

#elif BLOOM_PASS_4

	return textureSampleLevel(effect_sourceTexture, effect_sourceSampler, in.texCoords, 0.0);

#endif
}

)"
