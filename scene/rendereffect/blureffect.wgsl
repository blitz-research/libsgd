R"(

struct BlurEffectUniforms {
    texCoordScale: vec2f,
    kernelSize: u32,
    padding: u32,
    kernel: array<vec4f, 64>,
}

@group(2) @binding(0) var<uniform> effect_uniforms: BlurEffectUniforms;
@group(2) @binding(1) var effect_sourceTexture: texture_2d<f32>;
@group(2) @binding(2) var effect_sourceSampler: sampler;

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

    let size = effect_uniforms.kernelSize;

	var texCoords = in.texCoords - effect_uniforms.texCoordScale * f32(size) / 2.0;

	var color = vec3f(0.0);

	for(var i = 0u; i < size; i+=1) {

	    color += textureSampleLevel(effect_sourceTexture, effect_sourceSampler, texCoords, 0).rgb * effect_uniforms.kernel[i].w;

	    texCoords += effect_uniforms.texCoordScale;
	}

	return vec4f(color, 1);
}

)"
