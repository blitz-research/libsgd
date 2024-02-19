R"(

struct Varying {
    @builtin(position) position : vec4<f32>,
    @location(0) texCoords : vec2<f32>,
};

@vertex fn vertexMain(@builtin(vertex_index) vertexIndex : u32) -> Varying {

    const vertices = array<vec2<f32>, 4>(
        vec2<f32>(-1, 1), vec2<f32>(1, 1),
        vec2<f32>(-1, -1), vec2<f32>(1, -1));

    var out : Varying;

    out.position = vec4<f32>(vertices[vertexIndex], 0, 1);
    out.texCoords = out.position.xy * vec2<f32>(0.5, -0.5) + 0.5;

    return out;
}

@group(0) @binding(0) var srcTexture : texture_2d<f32>;
@group(0) @binding(1) var srcSampler : sampler;

@fragment fn fragmentMain(@location(0) texCoords : vec2<f32>) -> @location(0) vec4<f32> {

    return textureSample(srcTexture, srcSampler, texCoords);
}

)"
