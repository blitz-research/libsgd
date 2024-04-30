R"(

// ***** DrawListRenderer *****

struct DrawListUniforms {
    matrix: mat4x4f,
}
@group(2) @binding(0) var<uniform> renderer_uniforms: DrawListUniforms;

struct Vertex {
	@location(0) position: vec3f,
	@location(1) texCoords: vec2f,
	@location(2) color: vec4f,
};

struct Varying {
	@builtin(position) clipPosition: vec4f,
	@location(0) position: vec3f,
	@location(1) texCoords: vec2f,
	@location(2) color: vec4f,
};

@vertex fn vertexMain(vertex: Vertex) -> Varying {

    var out:Varying;

    out.clipPosition = renderer_uniforms.matrix * vec4f(vertex.position, 1);
    out.position = vertex.position;
    out.texCoords = vertex.texCoords;
    out.color = vertex.color;

	return out;
}

@fragment fn fragmentMain(in: Varying) -> @location(0) vec4f {

    var tanMatrix: mat3x3f;
    tanMatrix[2] = vec3f(0,0,1);

    let color = evaluateMaterial(in.position, tanMatrix, in.texCoords, in.color);

    return color;
}

)"
