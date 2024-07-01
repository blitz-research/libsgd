R"(

// ***** MeshRenderer *****

struct MeshInstance {
    worldMatrix: mat4x4f,
    color: vec4f,
}

struct MeshUniforms {
    tangentsEnabled: i32,
}

@group(2) @binding(0) var<uniform> meshUniforms: MeshUniforms;
@group(2) @binding(1) var<storage> meshInstances: array<MeshInstance>;

struct Vertex {
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) tangent: vec4f,
	@location(3) texCoords: vec3f,
	@location(4) color: vec4f,
};

struct Varying {
	@builtin(position) clipPosition: vec4f,
	@location(0) position: vec3f,
	@location(2) tanMatrix0: vec3f,
	@location(3) tanMatrix1: vec3f,
	@location(4) tanMatrix2: vec3f,
	@location(5) texCoords: vec3f,
	@location(6) color: vec4f,
};

@vertex fn vertexMain(vertex: Vertex, @builtin(instance_index) instanceId : u32) -> Varying {

    let instance = &meshInstances[instanceId];

	let worldMatrix = (*instance).worldMatrix;
	let normalMatrix = mat3x3f(worldMatrix[0].xyz, worldMatrix[1].xyz, worldMatrix[2].xyz);
	let mvpMatrix = cameraUniforms.viewProjectionMatrix * worldMatrix;

	var out: Varying;

	out.clipPosition = mvpMatrix * vec4f(vertex.position, 1.0);
	out.position = (worldMatrix * vec4f(vertex.position, 1.0)).xyz;
	if meshUniforms.tangentsEnabled != 0 {
        out.tanMatrix2 = normalMatrix * vertex.normal;
        out.tanMatrix0 = normalMatrix * vertex.tangent.xyz;
        out.tanMatrix1 = cross(out.tanMatrix0, out.tanMatrix2) * vertex.tangent.w;
    } else {
    	out.tanMatrix2 = normalMatrix * vertex.normal;
    }
	out.texCoords = vertex.texCoords;
	out.color = instance.color * vertex.color;

	return out;
}

@fragment fn fragmentMain(in: Varying) -> @location(0) vec4f {
#if !RENDER_PASS_SHADOW

    var tanMatrix: mat3x3f;

    if meshUniforms.tangentsEnabled != 0 {
        tanMatrix = mat3x3f(normalize(in.tanMatrix0), normalize(in.tanMatrix1), normalize(in.tanMatrix2));
    }else{
        tanMatrix[2] = in.tanMatrix2;
    }

    return evaluateMaterial(in.position, tanMatrix, in.texCoords, in.color);

#else

    return vec4f(0);

#endif
}

)"
