R"(

override srgbTarget = false;

@group(0) @binding(0) var srcTexture: texture_2d<f32>;
@group(0) @binding(1) var srcSampler: sampler;

struct Varying {
    @builtin(position) clipPosition: vec4f,
    @location(0) srcCoords: vec2f
};

@vertex fn vertexMain(@builtin(vertex_index) vertexId: u32) -> Varying {

    const vertices = array<vec2f, 4>(vec2f(-1, 1), vec2f(1, 1), vec2f(-1, -1), vec2f(1, -1));

    let vertex = vertices[vertexId];

    var out: Varying;

    out.clipPosition = vec4f(vertex, 0, 1);
    out.srcCoords = vertex * vec2f(.5, -.5) + .5f;

    return out;
}

@fragment fn fragmentMain(@location(0) srcCoords : vec2f) -> @location(0) vec4f {

    let color = textureSample(srcTexture, srcSampler, srcCoords);

    if !srgbTarget {
        return vec4f(pow(color.rgb, vec3f(1.0  /2.2)), color.a);
    }

    return color;
}

)"
