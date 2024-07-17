R"(

struct MonocolorEffectUniforms {
    color: vec4f,
}

@group(2) @binding(0) var<uniform> effect_uniforms: MonocolorEffectUniforms;
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

    let color = textureSampleLevel(effect_sourceTexture, effect_sourceSampler, in.texCoords, 0);

    let i = (color.r + color.g + color.b) / 3.0;

    return vec4f(i, i, i, 1) * effect_uniforms.color;
}

)"
