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

@fragment fn fragmentMain(in: Varying) -> @location(0) vec4<f32> {

    let color = textureSampleLevel(effect_sourceTexture, effect_sourceSampler, in.texCoords, 0);

#if BLOOM_PASS0

	fragColor = texture(sourceTexture, texCoords);
	fragColor.rgb = max(fragColor.rgb - vec3(1.0), vec3(0.0));

#elif BLOOM_PASS1

	fragColor = texture(sourceTexture, texCoords);

#elif BLOOM_PASS2 || BLOOM_PASS3

	const int size = 2;
	const float[size] offsets = float[size](0.53805, 2.06278);
	const float[size] weights = float[size](0.44908, 0.05092);

//	const int size = 9;
//	const float[size] offsets = float[size](0.66293, 2.47904, 4.46232, 6.44568, 8.42917, 10.41281, 12.39664, 14.38070, 16.36501);
//	const float[size] weights = float[size](0.10855, 0.13135, 0.10406, 0.07216, 0.04380, 0.02328, 0.01083, 0.00441, 0.00157);

#if BLOOM_PASS2
	vec2 halfTexelSize = vec2(1.0 / float(textureSize(sourceTexture, 0).x), 0.0);
#else
	vec2 halfTexelSize = vec2(0.0, 1.0 / float(textureSize(sourceTexture, 0).y));
#endif


	vec3 color = vec3(0.0);

	for(int i=0; i < size; ++i) {

		vec2 offset = halfTexelSize * offsets[i];

		color += texture(sourceTexture, texCoords + offset).rgb * weights[i];
		color += texture(sourceTexture, texCoords - offset).rgb * weights[i];
	}

	fragColor=vec4(color, 1.0);

#elif BLOOM_PASS4

	fragColor = texture(sourceTexture, texCoords);

#endif
}

)"
