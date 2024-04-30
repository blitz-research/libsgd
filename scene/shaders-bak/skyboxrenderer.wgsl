R"(

struct SkyboxUniforms {
    worldMatrix: mat4x4f,
    mipmapBias: f32
}

@group(2) @binding(0) var<uniform> skybox_uniforms: SkyboxUniforms;
@group(2) @binding(1) var skybox_skyboxTexture: texture_cube<f32>;
@group(2) @binding(2) var skybox_skyboxSampler: sampler;

struct Varying {
    @builtin(position) clipPosition: vec4f,
    @location(0) clipCoords: vec2f
}

@vertex fn vertexMain(@builtin(vertex_index) vertexId: u32) -> Varying {

    const vertices = array<vec2f, 4>(vec2f(-1.0, 1.0), vec2f(1.0, 1.0), vec2f(-1.0, -1.0), vec2f(1.0, -1.0));

    let vertex = vertices[vertexId];

    var out : Varying;

    out.clipPosition = vec4f(vertex, 0.0, 1.0);
	out.clipCoords = vertex;

    return out;
}

@fragment fn fragmentMain(in: Varying) -> @location(0) vec4<f32> {

    let tv = camera_uniforms.inverseProjectionMatrix * vec4f(in.clipCoords, 0, 1);

    let wv = camera_uniforms.worldMatrix * skybox_uniforms.worldMatrix * vec4f(tv.xyz / tv.w, 0);

    return textureSampleBias(skybox_skyboxTexture, skybox_skyboxSampler, wv.xyz, skybox_uniforms.mipmapBias);
}

)"
