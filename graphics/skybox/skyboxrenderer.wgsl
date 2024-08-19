R"(

struct SkyboxUniforms {
    worldMatrix: mat4x4f,
    mipmapBias: f32,
    textureType: u32,
}

@group(2) @binding(0) var<uniform> geometry_uniforms: SkyboxUniforms;
@group(2) @binding(1) var geometry_skyboxTextureCube: texture_cube<f32>;
@group(2) @binding(2) var geometry_skyboxTexture2D: texture_cube<f32>;
@group(2) @binding(3) var geometry_skyboxSampler: sampler;

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

@fragment fn fragmentMain(in: Varying) -> @location(0) vec4f {

    let ndcCoords = in.clipCoords;

    let tvec = scene_camera.inverseProjectionMatrix * vec4f(ndcCoords, 0, 1);
    let wvec = (scene_camera.worldMatrix * geometry_uniforms.worldMatrix * vec4f(tvec.xyz/ tvec.w, 0)).xyz;

    if(geometry_uniforms.textureType == 1) {

        return textureSampleBias(geometry_skyboxTextureCube, geometry_skyboxSampler, wvec, geometry_uniforms.mipmapBias);

    } else {

        let u = atan2(wvec.x, wvec.z) / pi * 0.5 + 0.5;
        let v = -atan2(wvec.y, length(wvec.xz)) / pi + 0.5;

//        return textureSampleBias(scene_envTexture2D, scene_envSampler, vec2f(u, v), geometry_uniforms.mipmapBias);
        return textureSampleLevel(scene_envTexture2D, scene_envSampler, vec2f(u, v), geometry_uniforms.mipmapBias);
    }
}

)"
