R"(

struct TerrainUniforms {
    lodLevels: u32,
    quadsPerTile: u32,
//    meshScale: f32,
//    heightScale: f32,
};

@group(2) @binding(0) var<uniform> terrainUniforms: TerrainUniforms;
@group(2) @binding(1) var terrainHeightTexture: texture_2d<f32>;
@group(2) @binding(2) var terrainHeightSampler: sampler;
//@group(2) @binding(3) var terrainNormalTexture: texture_2d<f32>;
//@group(2) @binding(4) var terrainNormalSampler: sampler;

struct Vertex {
    @location(0) position: vec3f,
    @location(1) color: vec3f
}

struct Varying {
    @builtin(position) clipPosition: vec4f,
    @location(0) position: vec3f,
    @location(1) normal: vec3f,
    @location(2) texCoords: vec2f,
    @location(3) color: vec4f
}

/*
const lods = array<vec2f>(
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

const offs = array<vec4f>(
	vec4f(0.0), vec4f(-1.0,0.0, 1.0,0.0), vec4f(0.0),
	vec4f(0.0,-1.0, 0.0,1.0), vec4f(0.0), vec4f(0.0,-1.0, 0.0,1.0),
	vec4f(0.0), vec4f(-1.0,0.0, 1.0,0.0), vec4f(0.0),
	vec4f(-.5,.5f, .5,-.5), vec4f(-.5,-.5, .5,.5),
	vec4f(-.5,-.5, .5,.5), vec4f(-.5,.5, .5,-.5)
);
*/

@vertex fn vertexMain(vertex: Vertex, @builtin(vertex_index) vertexId: u32) -> Varying {

    let position = vertex.position;
    let texCoords = position.xz;

    var out: Varying;
    out.clipPosition = scene_camera.viewProjectionMatrix * vec4f(position, 1);
    out.position = position;
    out.normal = vec3f(0, 1, 0);
    out.texCoords = texCoords;
    out.color = vec4f(vertex.color, 1);

    return out;
/*

    let tileSize = float(terrain.quadsPerTile);

    let eye = camera.world[3].xyz;

    let pos = round(eye / tileSize) * tileSize + vertex.position;
    pos.y = 0.0;

    // ***** Compute LODs *****

    let v = pos - eye;
    let d = max(abs(v.x), abs(v.z)) / tileSize - 0.5;

    let i = min(int(floor(d)) + 1, 64);

    let lodNear = lods[i].s;
    let tween = lods[i].t * fract(d);

	if(lodNear == vertex.position.y + 1 && tween == 0) {
		lodNear = vertex.position.y;
		tween = 1;
	}else if(lodNear != vertex.position.y) {
		out.color = vec3f(1, 0, 0);
		return;
	}

	let lodFar = lodNear + 1;

    // ***** Compute height *****
    //
    let off = offs[vertexId % 13] * exp2(lodNear);

    let texCoords = pos.xz;

    let heightNear = textureSampleLevel(terrain_heightTexture, terrain_heightSampler, texCoords0, lodNear).r;
    let height0 = textureSampleLevel(terrain_heightTexture, terrain_heightSampler, (texCoords0 + off.xy), lodFar).r;
    let height1 = textureSampleLevel(terrain_heightTexture, terrain_heightSampler, (texCoords0 + off.zw), lodFar).r;
    let heightFar = mix(height0, height1, .5);

    pos.y = mix(heightNear, heightFar, tween);

    // ***** Done! *****
    //
    var out: VertexOut;
    out.clipPosition = camera.viewProjectionMatrix * pos;
    out.positon = pos;
    out.normal = vec3f(0, 1, 0);
    out.tCoords = texCoords;
    out.color = vec4f(vertex.color, 1);
*/
}

@fragment fn fragmentMain(in: Varying) -> @location(0) vec4f {

//    let fragColor = evaluateFragment(in.wPosition, normalize(in.wNormal), in.tCoords, in.color);

//    return fragColor;
    return in.color;
//    return vec4(1,1,0,1);
}

)"
