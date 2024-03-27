R"(

// ***** MeshRenderer *****

struct MeshUniforms {
    tangentsEnabled: i32,
}
@group(2) @binding(0) var<uniform> mesh_uniforms: MeshUniforms;

struct Vertex {
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) tangent: vec4f,
	@location(3) texCoords: vec2f,
	@location(4) color: vec4f,
};

struct MeshInstance {
	@location(8) matrix_0: vec4f,
	@location(9) matrix_1: vec4f,
	@location(10) matrix_2: vec4f,
	@location(11) matrix_3: vec4f,
	@location(12) color: vec4f,
};

struct Varying {
	@builtin(position) clipPosition: vec4f,
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) tanMatrix0: vec3f,
	@location(3) tanMatrix1: vec3f,
	@location(4) tanMatrix2: vec3f,
	@location(5) texCoords: vec2f,
	@location(6) color: vec4f,
};

@vertex fn vertexMain(vertex: Vertex, instance: MeshInstance) -> Varying {

	let meshMatrix = mat4x4f(instance.matrix_0, instance.matrix_1, instance.matrix_2, instance.matrix_3);
	let normalMatrix = mat3x3f(meshMatrix[0].xyz, meshMatrix[1].xyz, meshMatrix[2].xyz);
	let mvpMatrix = camera_uniforms.viewProjectionMatrix * meshMatrix;

	var out: Varying;

	out.clipPosition = mvpMatrix * vec4f(vertex.position, 1.0);
	out.position = (meshMatrix * vec4f(vertex.position, 1.0)).xyz;
	out.texCoords = vertex.texCoords;
	out.color = instance.color * vertex.color;

	if mesh_uniforms.tangentsEnabled != 0 {
//        out.tanMatrix2 = normalize(normalMatrix * vertex.normal);
//        out.tanMatrix0 = normalize(normalMatrix * vertex.tangent.xyz);
//        out.tanMatrix1 = cross(out.tanMatrix0, out.tanMatrix2) * vertex.tangent.w;
        out.tanMatrix2 = normalMatrix * vertex.normal;
        out.tanMatrix0 = normalMatrix * vertex.tangent.xyz;
        out.tanMatrix1 = cross(out.tanMatrix0, out.tanMatrix2) * vertex.tangent.w;
    } else {
    	out.normal = normalize(normalMatrix * vertex.normal);
    }

	return out;
}

@fragment fn fragmentMain(in: Varying) -> @location(0) vec4f {

    let material = evaluateMaterial(in.position, in.normal, in.texCoords, in.color);

    var normal:vec3f;

    if mesh_uniforms.tangentsEnabled != 0 {
//        let tanMatrix = mat3x3f(in.tanMatrix0, in.tanMatrix1, in.tanMatrix2);
        let tanMatrix = mat3x3f(normalize(in.tanMatrix0), normalize(in.tanMatrix1), in.tanMatrix2);
        normal = normalize(tanMatrix * material.normal);
    }else{
        normal = normalize(in.normal);
    }

    let lighting = evaluateLighting(in.position, normal, material.albedo.rgb, material.metallic, material.roughness, material.occlusion);

    return vec4f(lighting + material.emissive, material.albedo.a);
}

)"
