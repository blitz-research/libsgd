R"(

struct ImageUniforms {
    rect: vec4f,
    viewMode: u32,
}

@group(2) @binding(0) var<uniform> geometry_uniforms: ImageUniforms;

struct ImageInstance {
    worldMatrix: mat4x4f,
    color: vec4f,
    frame: f32,
}

@group(3) @binding(0) var<storage> renderer_instances: array<ImageInstance>;

struct Varying {
    @builtin(position) clipPosition : vec4f,
    @location(0) position: vec3f,
    @location(1) texCoords: vec3f,
    @location(2) color: vec4f,
}

@vertex fn vertexMain(@builtin(vertex_index) vertexId : u32) -> Varying {

    const vertices = array<vec2f, 6>(vec2f(0, 1), vec2f(1, 0), vec2f(1, 1), vec2f(0, 1), vec2f(0, 0), vec2f(1, 0));

    let instance = renderer_instances[vertexId / 6];
    let vcoords = vertices[vertexId % 6];

    // Very cheeky! Will only work with imagematerial...
    let rect = geometry_uniforms.rect;

    let vertex = vec3f(rect.zw * vcoords + rect.xy, 0);

    var position:vec3f;

    // Note: Might be better to just store trans/basis/scle

    if geometry_uniforms.viewMode == 1 {  // fixed, locked to camera

        let scale = vec3f(length(instance.worldMatrix[0]), length(instance.worldMatrix[1]), length(instance.worldMatrix[2]));

        position = (scene_camera.worldMatrix * vec4f(scale * vertex, 0)).xyz + instance.worldMatrix[3].xyz;

    } else if geometry_uniforms.viewMode == 2 {   // free, billboard style

        position = (instance.worldMatrix * vec4f(vertex, 1)).xyz;

    } else if geometry_uniforms.viewMode == 3 {   // upright, tree style

        let scale = vec3f(length(instance.worldMatrix[0]), length(instance.worldMatrix[1]), length(instance.worldMatrix[2]));

        var basis: mat3x3f;
        basis[1] = vec3f(0, 1, 0);//instance.worldMatrix[1].xyz / scale[1];
        basis[0] = normalize(cross(scene_camera.worldMatrix[3].xyz - instance.worldMatrix[3].xyz, basis[1]));
        basis[2] = normalize(cross(basis[0], basis[1]));

        position = (basis * (scale * vertex)) + instance.worldMatrix[3].xyz;

    } else {    // default to billboard style

        position = (instance.worldMatrix * vec4f(vertex, 1)).xyz;
    }

    var out: Varying;

    out.clipPosition = scene_camera.viewProjectionMatrix * vec4(position, 1);
    out.position = position;
    out.texCoords = vec3f(1 - vcoords, instance.frame);
    out.color = instance.color;

    return out;
}

@fragment fn fragmentMain(in: Varying) -> @location(0) vec4f {

#if !RENDER_PASS_SHADOW

    return evaluateMaterial(in.position, mat3x3f(), in.texCoords, in.color);

#else

    // Sprite can't cast shadows yet, so this should be unreachable.
    // return vec4f(0);

#endif
}

)"
