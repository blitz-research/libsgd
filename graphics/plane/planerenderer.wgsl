R"(

struct PlaneUniforms {
    worldMatrix: mat4x4f,
    invWorldMatrix: mat4x4f,
    worldNormal: vec4f,
    color: vec4f,
}

@group(2) @binding(0) var<uniform> geometry_plane: PlaneUniforms;

struct Varying {
    @builtin(position) clipPosition: vec4f,
    @location(0) clipCoords: vec2f,
    @location(1) tanMatrix0: vec3f,
    @location(2) tanMatrix1: vec3f,
    @location(3) tanMatrix2: vec3f,
}

@vertex fn vertexMain(@builtin(vertex_index) vertexId: u32) -> Varying {

    const vertices = array<vec2f, 4>(vec2f(-1.0, 1.0), vec2f(-1.0, -1.0), vec2f(1.0, 1.0), vec2f(1.0, -1.0));
    const tangents = array<vec3f, 4>(vec3f(1.0, 0.0, 0.0), vec3f(0.0, 0.0, 1.0), vec3f(-1.0, 0.0, 0.0), vec3f(0.0, 0.0, -1.0));

    let vertex = vertices[vertexId];

    var out : Varying;

    out.clipPosition = vec4f(vertex, 0.0, 1.0);
	out.clipCoords = vertex;

    out.tanMatrix2 = vec3f(0,1,0);
    out.tanMatrix0 = tangents[vertexId];
    out.tanMatrix1 = cross(out.tanMatrix0, out.tanMatrix2);

    return out;
}

struct Fragment {
    @builtin(frag_depth) fragDepth: f32,
    @location(0) color: vec4f,
};

@fragment fn fragmentMain(in: Varying) -> Fragment {

    // OK, this ain't too pretty, we should really should just do plane intersection in view space.

    var tpos = scene_camera.inverseProjectionMatrix * vec4f(in.clipCoords, 0, 1);
    tpos /= tpos.w;

    let worldDir = scene_camera.worldMatrix * vec4f(tpos.xyz, 0);

    let eye = geometry_plane.invWorldMatrix * scene_camera.worldMatrix[3];
    if eye.y < 0 { discard; }

    let dir = geometry_plane.invWorldMatrix * worldDir;
    if dir.y >= 0 { discard; }

    let t = -eye.y / dir.y;
    let pos = eye + dir * t;

    let worldPos = geometry_plane.worldMatrix * pos;

    let wpos = scene_camera.viewProjectionMatrix * worldPos;
    let spos = wpos.xyz / wpos.w;
    if spos.z >=1 { discard; }

    let tanMatrix = mat3x3f(normalize(in.tanMatrix0), normalize(in.tanMatrix1), normalize(in.tanMatrix2));

    var out:Fragment;

    out.fragDepth = spos.z;
    out.color = evaluateMaterial(worldPos.xyz, tanMatrix, vec3f(pos.xz, 0), geometry_plane.color);

    return out;
}

)"
