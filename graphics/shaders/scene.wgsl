R"(

// ***** Camera *****

struct CameraUniforms {
	projectionMatrix: mat4x4f,
	inverseProjectionMatrix: mat4x4f,
	viewProjectionMatrix: mat4x4f,
	worldMatrix: mat4x4f,
	viewMatrix: mat4x4f,
	clipNear: f32,
	clipFar: f32,
};

@group(0) @binding(0) var<uniform> camera_uniforms: CameraUniforms;

// ***** Lighting *****

const maxPointLights: u32 = 16;
const maxDirectionalLights: u32 = 4;

struct PointLight {
	position: vec4f,
 	color: vec4f,
	range: f32,
	falloff: f32,
}

struct DirectionalLight {
    direction: vec4f,
    color: vec4f,
}

struct LightingUniforms {
	ambientLightColor: vec4f,
	numPointLights: u32,
	pointLights: array<PointLight, maxPointLights>,
	numDirectionalLights: u32,
	directionalLights: array<DirectionalLight, maxDirectionalLights>,
}

@group(0) @binding(1) var<uniform> lighting_uniforms: LightingUniforms;
@group(0) @binding(2) var lighting_envTexture: texture_cube<f32>;
@group(0) @binding(3) var lighting_envSampler: sampler;

fn evaluateLighting(position: vec3f, normal: vec3f, albedo: vec3f, metallic: f32, roughness: f32, occlusion: f32) -> vec3f {

	const black = vec3(0.04, 0.04, 0.04);

	let diffuse = albedo * (1.0 - metallic);
	let specular = (albedo - black) * metallic + black;
	let glossiness = 1.0 - roughness;

	let vpos = camera_uniforms.worldMatrix[3].xyz;  // viewer position
    let vvec = normalize(vpos - position);          // vector to viewer
	let ndotv = max(dot(normal, vvec), 0.0);

    let spower = pow(2.0, glossiness * 12.0);       // specular power
    let fnorm = (spower + 2.0) / 8.0;               // normalization factor

    let ambdiffuse = lighting_uniforms.ambientLightColor.rgb * lighting_uniforms.ambientLightColor.a * diffuse;

    let mips = f32(textureNumLevels(lighting_envTexture));
    let tv = reflect(-vvec, normal);
    let env = textureSampleLevel(lighting_envTexture, lighting_envSampler, tv, roughness * mips).rgb;
	let fschlick0 = specular + (1.0 - specular) * pow(1.0 - ndotv, 5.0) * glossiness;

	let ambspecular = fschlick0 * env;

	var color = (ambdiffuse + ambspecular) * occlusion;

	for(var i: u32 = 0; i < lighting_uniforms.numPointLights; i += 1) {
	    let light = lighting_uniforms.pointLights[i];

        // vector to light
	    var lvec = light.position.xyz - position;
	    let d = length(lvec);
	    if d >= light.range {continue;}
	    lvec /= d;

        // Attenuation - this is seems like the most practially useful:
        // https://lisyarus.github.io/blog/graphics/2022/07/30/point-light-attenuation.html
        const F = 1.0;
        let s = d / light.range;
        let s2 = 1.0 - s * s;
        let atten = light.color.a * s2 * s2 / (1.0 + light.falloff * s);

        // Dot products
    	let hvec = normalize(lvec + vvec);
        let ndotl = max(dot(normal, lvec), 0.0);
        let ndoth = max(dot(normal, hvec), 0.0);
        let hdotl = max(dot(hvec, lvec ), 0.0);

	    let fschlick = specular + (1.0 - specular) * pow(1.0 - hdotl, 5.0) * glossiness;
	    let fspecular = fschlick * pow(ndoth, spower) * fnorm;

	    let illum = light.color.rgb * ndotl * atten;

	    color += (diffuse + fspecular) * illum;
	}

	for(var i: u32 = 0; i < lighting_uniforms.numDirectionalLights; i += 1) {
	    let light = lighting_uniforms.directionalLights[i];
	}

	return color;
}

// ***** Material *****

// fn evaluateMaterial(position: vec3f, normal: vec3f, texCoords: vec2f, color: vec4f) -> MaterialResult;
struct MaterialResult {
    albedo: vec4f,
    emissive: vec3f,
    metallic: f32,
    roughness: f32,
    occlusion: f32,
    normal: vec3f,
}

)"
