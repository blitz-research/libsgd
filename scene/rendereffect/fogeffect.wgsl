R"(

struct FogEffectUniforms {
    color: vec4f,
    near: f32,
    range: f32,    // far - near
    power: f32,
}

@group(2) @binding(0) var<uniform> effect_uniforms: FogEffectUniforms;
@group(2) @binding(1) var effect_sourceTexture: texture_2d<f32>;
@group(2) @binding(2) var effect_sourceSampler: sampler;
@group(2) @binding(3) var effect_depthTexture: texture_depth_2d;
@group(2) @binding(4) var effect_depthSampler: sampler;

struct Varying {
    @builtin(position) clipPosition: vec4f,
    @location(0) clipCoords: vec4f,
    @location(1) texCoords: vec2f,
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

    // Depth buffer depth
    let tz = textureSampleLevel(effect_depthTexture, effect_depthSampler, in.texCoords, 0);

    if tz == 1.0 {return color;}

    let far = scene_camera.clipFar;
    let near = scene_camera.clipNear;

    // View Z coordinate
    let vz = (far * near) / (near * tz - far * tz + far);

    let alpha = pow( min(max(vz - effect_uniforms.near, 0.0) / effect_uniforms.range, 1.0), effect_uniforms.power);

    return mix(color, effect_uniforms.color, alpha);
}

)"
