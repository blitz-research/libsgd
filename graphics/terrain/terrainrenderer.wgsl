R"(

struct TerrainUniforms {
    worldMatrix: mat4x4f,
    size: u32,
    lods: u32,
    quadsPerTile: u32,
    debugMode: u32,
    materialTexelSize: f32,
    heightTexelSize: f32,
};

@group(2) @binding(0) var<uniform> terrainUniforms: TerrainUniforms;
@group(2) @binding(1) var terrainHeightTexture: texture_2d<f32>;
@group(2) @binding(2) var terrainHeightSampler: sampler;
@group(2) @binding(3) var terrainNormalTexture: texture_2d<f32>;
@group(2) @binding(4) var terrainNormalSampler: sampler;

struct Vertex {
    @location(0) position: vec3f,
    @location(1) color: vec3f
}

struct Varying {
    @builtin(position) clipPosition: vec4f,
    @location(0) position: vec3f,
    @location(1) normalTexCoords: vec2f,
    @location(2) materialTexCoords: vec2f,
    @location(3) lod: f32,
    @location(4) color: vec4f
}

const lods = array<vec2f, 65>(
	vec2f(0.0, 0.0), vec2f(0.0, 1.0),

	vec2f(1.0, 0.0), vec2f(1.0, 1.0),

	vec2f(2.0, 0.0), vec2f(2.0, 0.0), vec2f(2.0, 0.0), vec2f(2.0, 1.0),

	vec2f(3.0, 0.0), vec2f(3.0, 0.0), vec2f(3.0, 0.0), vec2f(3.0, 0.0), vec2f(3.0, 0.0), vec2f(3.0, 0.0), vec2f(3.0, 0.0), vec2f(3.0, 1.0),

	vec2f(4.0, 0.0), vec2f(4.0, 0.0), vec2f(4.0, 0.0), vec2f(4.0, 0.0), vec2f(4.0, 0.0), vec2f(4.0, 0.0), vec2f(4.0, 0.0), vec2f(4.0, 0.0),
	vec2f(4.0, 0.0), vec2f(4.0, 0.0), vec2f(4.0, 0.0), vec2f(4.0, 0.0), vec2f(4.0, 0.0), vec2f(4.0, 0.0), vec2f(4.0, 0.0), vec2f(4.0, 1.0),

	vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0),
	vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0),
	vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0),
	vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 0.0), vec2f(5.0, 1.0),

	vec2f(6.0, 0.0)
);

const offs = array<vec4f, 13>(
	vec4f(0.0),  vec4f(-1.0,0.0, 1.0,0.0), vec4f(0.0),
	vec4f(0.0,-1.0, 0.0,1.0), vec4f(0.0), vec4f(0.0,-1.0, 0.0,1.0),
	vec4f(0.0), vec4f(-1.0,0.0, 1.0,0.0), vec4f(0.0),
	vec4f(-.5,.5f, .5,-.5), vec4f(-.5,-.5, .5,.5),
	vec4f(-.5,-.5, .5,.5), vec4f(-.5,.5, .5,-.5)
);

@vertex fn vertexMain(vertex: Vertex, @builtin(vertex_index) vertexId: u32) -> Varying {

    let mvpMatrix = scene_camera.viewProjectionMatrix * terrainUniforms.worldMatrix;

    let tileSize = f32(terrainUniforms.quadsPerTile);

    var color = vertex.color;

    // ***** Compute position *****

    let eye = -terrainUniforms.worldMatrix[3].xyz;
    var pos = round(eye / tileSize) * tileSize + vertex.position;
    pos.y = 0.0;

    // ***** Compute LODs *****

    let v = pos - eye;
    let d = max(abs(v.x), abs(v.z)) / tileSize - 0.5;

    let i = min(i32(floor(d)) + 1, 64);

    var lodNear = lods[i].x;
    var tween = lods[i].y * fract(d);

	if(lodNear == vertex.position.y + 1 && tween == 0) {
		lodNear = vertex.position.y;
		tween = 1;
	}else if(lodNear != vertex.position.y) {
	    color = vec3f(1,0,1);
	}
	let lodFar = lodNear + 1;

    // ***** Compute height *****

    let off = offs[vertexId % 13] * exp2(lodNear);

    let texCoords = pos.xz;
    let texelSize = 1.0 / vec2f(textureDimensions(terrainHeightTexture));

    let heightNear = textureSampleLevel(terrainHeightTexture, terrainHeightSampler, texCoords * texelSize + 0.5, lodNear).r;
    let height0 = textureSampleLevel(terrainHeightTexture, terrainHeightSampler, (texCoords + off.xy) * texelSize + 0.5, lodFar).r;
    let height1 = textureSampleLevel(terrainHeightTexture, terrainHeightSampler, (texCoords + off.zw) * texelSize + 0.5, lodFar).r;
    let heightFar = mix(height0, height1, .5);

    pos.y = mix(heightNear, heightFar, tween);

	// Output fragment
    //
    var out: Varying;
    out.clipPosition = mvpMatrix * vec4f(pos, 1.0);
    out.position = (terrainUniforms.worldMatrix * vec4f(pos, 1.0)).xyz;
    out.normalTexCoords = texCoords / vec2f(textureDimensions(terrainNormalTexture)) + 0.5;
    out.materialTexCoords = texCoords * terrainUniforms.materialTexelSize + 0.5;
    out.lod = lodNear + tween;
    out.color = vec4f(color, 1.0);
    //
    return out;
}

@fragment fn fragmentMain(in: Varying) -> @location(0) vec4f {
#if !RENDER_PASS_SHADOW

    if terrainUniforms.debugMode != 0 {
        return in.color;
    }

    var tanMatrix: mat3x3f;
    tanMatrix[2] = normalize(textureSampleLevel(terrainNormalTexture, terrainNormalSampler, in.normalTexCoords, in.lod).rgb * 2.0 - 1.0).xzy;

    let color = evaluateMaterial(in.position, tanMatrix, vec3f(in.materialTexCoords, 0.0), vec4f(1));

    return color;

#else
    return vec4f(0);
#endif
}

)"
