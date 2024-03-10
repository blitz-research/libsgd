R"(

// ***** BonedMeshRenderer *****

struct MeshUniforms {
    tangentsEnabled: i32,
}

const maxJoints: u32 = 256;
struct SkinnedMeshInstance {
    worldMatrix: mat4x4f,
    color: vec4f,
    jointMatrices: array<mat4x4f, maxJoints>,
};

@group(2) @binding(0) var<uniform> mesh_uniforms: MeshUniforms;
@group(2) @binding(1) var<storage> skinnedMeshInstances: array<SkinnedMeshInstance>;

struct Vertex {
	@location(0) position: vec3f,
	@location(1) normal: vec3f,
	@location(2) tangent: vec4f,
	@location(3) texCoords: vec2f,
	@location(4) color: vec4f,
	@location(5) joints: vec4u,
	@location(6) weights: vec4f,
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

@vertex fn vertexMain(vertex: Vertex, @builtin(instance_index) instanceId : u32) -> Varying {

    let instance = &skinnedMeshInstances[instanceId];

    let joints = vertex.joints;

    let weights = vertex.weights;

    let matrix0 = (*instance).jointMatrices[joints.x];
    let matrix1 = (*instance).jointMatrices[joints.y];
    let matrix2 = (*instance).jointMatrices[joints.z];
    let matrix3 = (*instance).jointMatrices[joints.w];

    let position =
        (matrix0 * vec4(vertex.position, 1) * weights.x +
        matrix1 * vec4(vertex.position, 1) * weights.y +
        matrix2 * vec4(vertex.position, 1) * weights.z +
        matrix3 * vec4(vertex.position, 1) * weights.w).xyz;

    let normal =
        (matrix0 * vec4(vertex.normal, 0) * weights.x +
        matrix1 * vec4(vertex.normal, 0) * weights.y +
        matrix2 * vec4(vertex.normal, 0) * weights.z +
        matrix3 * vec4(vertex.normal, 0) * weights.w).xyz;

    let tangent = vec4f(
        (matrix0 * vec4(vertex.tangent.xyz, 0) * weights.x +
        matrix1 * vec4(vertex.tangent.xyz, 0) * weights.y +
        matrix2 * vec4(vertex.tangent.xyz, 0) * weights.z +
        matrix3 * vec4(vertex.tangent.xyz, 0) * weights.w).xyz, vertex.tangent.w);

	var out: Varying;

	out.clipPosition = camera_uniforms.viewProjectionMatrix * vec4f(position, 1);
	out.position = position;
	out.texCoords = vertex.texCoords;
	out.color = (*instance).color * vertex.color;

	if mesh_uniforms.tangentsEnabled != 0 {
        out.tanMatrix2 = normalize(normal);
        out.tanMatrix0 = normalize(tangent.xyz);
        out.tanMatrix1 = cross(out.tanMatrix0, out.tanMatrix2) * tangent.a;
    } else {
    	out.normal = normalize(normal);
    }

	return out;
}

@fragment fn fragmentMain(in: Varying) -> @location(0) vec4f {

    let material = evaluateMaterial(in.position, in.normal, in.texCoords, in.color);

    var normal:vec3f;

    if mesh_uniforms.tangentsEnabled != 0 {
        let tanMatrix = mat3x3f(in.tanMatrix0, in.tanMatrix1, in.tanMatrix2);
        normal = normalize(tanMatrix * material.normal);
    }else{
        normal = normalize(in.normal);
    }

    let lighting = evaluateLighting(in.position, normal, material.albedo.rgb, material.metallic, material.roughness, material.occlusion);

    return vec4f(lighting + material.emissive, material.albedo.a);
}

)"
