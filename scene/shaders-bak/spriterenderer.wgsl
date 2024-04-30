R"(

struct SpriteInstance {
    matrix: mat4x4f,
    color: vec4f,
    rect: vec4f,
}
@group(2) @binding(0) var<storage> spriteInstances: array<SpriteInstance>;

struct VertexOut {
    @builtin(position) clipPosition : vec4f,
    @location(0) position: vec3f,
    @location(1) normal: vec3f,
    @location(2) texCoords: vec2f,
    @location(3) color: vec4f,
}

@vertex fn vertexMain(@builtin(vertex_index) vertexId : u32) -> VertexOut {

    const vertices = array<vec2f, 6>(vec2f(0, 1), vec2f(1, 0), vec2f(1, 1), vec2f(0, 1), vec2f(0, 0), vec2f(1, 0));

    let instance = spriteInstances[vertexId / 6];
    let vcoords = vertices[vertexId % 6];

    var vertex = vec3f(
        (instance.rect.z - instance.rect.x) * vcoords.x + instance.rect.x,
        (instance.rect.w - instance.rect.y) * vcoords.y + instance.rect.y, 0);

    vertex = (camera_uniforms.worldMatrix * vec4f(vertex, 0)).xyz;

    var out: VertexOut;

    out.position = instance.matrix[3].xyz + vertex;
    out.normal = camera_uniforms.worldMatrix[3].xyz - instance.matrix[3].xyz;
    out.texCoords = 1 - vcoords;
    out.color = instance.color;

    out.clipPosition = camera_uniforms.viewProjectionMatrix * vec4(out.position, 1);

    return out;
}

@fragment fn fragmentMain(in: VertexOut) -> @location(0) vec4f {

    var tanMatrix: mat3x3f;
    tanMatrix[2] = normalize(in.normal);

    return evaluateMaterial(in.position, tanMatrix, in.texCoords, in.color);
}

)"
