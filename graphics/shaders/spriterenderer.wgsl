R"(

struct SpriteInstance {
    matrix: mat4x4f,
    color: vec4f,
    frame: f32,
}

@group(2) @binding(0) var<storage> spriteInstances: array<SpriteInstance>;

struct Varying {
    @builtin(position) clipPosition : vec4f,
    @location(0) position: vec3f,
    @location(1) normal: vec3f,
    @location(2) texCoords: vec2f,
    @location(3) color: vec4f,
}

@vertex fn vertexMain(@builtin(vertex_index) vertexId : u32) -> Varying {

    const vertices = array<vec2f, 6>(vec2f(0, 1), vec2f(1, 0), vec2f(1, 1), vec2f(0, 1), vec2f(0, 0), vec2f(1, 0));

    let instance = spriteInstances[vertexId / 6];
    let vcoords = vertices[vertexId % 6];

    // Very cheeky! Will only work with imagematerial...
    let rect = material_uniforms.rect;

    let vertex = vec3f(rect.zw * vcoords + rect.xy, 0);

    var position:vec3f;

    // Note: Might be better to just store trans/basis/scle

    if material_uniforms.spriteViewMode == 1 {  // fixed, locked to camera

        let scale = vec3f(length(instance.matrix[0]), length(instance.matrix[1]), length(instance.matrix[2]));

        position = (cameraUniforms.worldMatrix * vec4f(scale * vertex, 0)).xyz + instance.matrix[3].xyz;

    } else if material_uniforms.spriteViewMode == 2 {   // free, billboard style

        position = (instance.matrix * vec4f(vertex, 1)).xyz;

    } else if material_uniforms.spriteViewMode == 3 {   // upright, tree style

        let scale = vec3f(length(instance.matrix[0]), length(instance.matrix[1]), length(instance.matrix[2]));

        var basis: mat3x3f;
        basis[1] = instance.matrix[1].xyz / scale[1];
        basis[0] = normalize(cross(cameraUniforms.worldMatrix[3].xyz - instance.matrix[3].xyz, basis[1]));
        basis[2] = normalize(cross(basis[0], basis[1]));

        position = (basis * (scale * vertex)) + instance.matrix[3].xyz;

    } else {    // default to billboard style

        position = (instance.matrix * vec4f(vertex, 1)).xyz;
    }

    var out: Varying;

    out.clipPosition = cameraUniforms.viewProjectionMatrix * vec4(position, 1);
    out.position = position;
    out.texCoords = 1 - vcoords;
    out.color = instance.color;

    return out;
}

@fragment fn fragmentMain(in: Varying) -> @location(0) vec4f {

#if !RENDER_PASS_SHADOW

    var tanMatrix: mat3x3f;
    tanMatrix[2] = normalize(in.normal);

    return evaluateMaterial(in.position, tanMatrix, vec3f(in.texCoords, 0), in.color);

#else

    // Sprite shouldn't cast shadows yet, so this should be unreachable.
    // return vec4f(0);

#endif
}

)"
