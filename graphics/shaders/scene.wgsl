R"(

// If the uniformity analysis is wrong because it can't prove uniformity, but you want to assert that the control flow is actually uniform,
// in a fragment shader you can use:
// @diagnostic(off, derivative_uniformity)

// ***** Scene *****

const maxDirectionalLights: u32 = 4;
const maxPointLights: u32 = 32;
const maxSpotLights: u32 = 16;

struct CameraUniforms {
	projectionMatrix: mat4x4f,
	inverseProjectionMatrix: mat4x4f,
	viewProjectionMatrix: mat4x4f,
	worldMatrix: mat4x4f,
	viewMatrix: mat4x4f,
	clipNear: f32,
	clipFar: f32,
};

struct DirectionalLight {
    direction: vec3f,
    color: vec4f,
}

struct PointLight {
	position: vec3f,
 	color: vec4f,
	range: f32,
	falloff: f32,
 	castsShadow: i32,
}

struct SpotLight {
	position: vec3f,
    direction: vec3f,
 	color: vec4f,
	range: f32,
	falloff: f32,
	innerConeAngle: f32,
	outerConeAngle: f32,
}

struct LightingUniforms {
	ambientLightColor: vec4f,
	numDirectionalLights: u32,
	directionalLights: array<DirectionalLight, maxDirectionalLights>,
	numPointLights: u32,
	pointLights: array<PointLight, maxPointLights>,
	numSpotLights: u32,
	spotLights: array<SpotLight, maxSpotLights>,
}

@group(0) @binding(0) var<uniform> camera_uniforms: CameraUniforms;
@group(0) @binding(1) var<uniform> lighting_uniforms: LightingUniforms;
@group(0) @binding(2) var lighting_envTexture: texture_cube<f32>;
@group(0) @binding(3) var lighting_envSampler: sampler;
@group(0) @binding(4) var lighting_csmTexture: texture_2d_array<f32>;
@group(0) @binding(5) var lighting_csmSampler: sampler;
@group(0) @binding(6) var lighting_psmTexture: texture_depth_cube_array;
@group(0) @binding(7) var lighting_psmSampler: sampler;

fn pointLightAtten(d: f32, range: f32, falloff: f32) -> f32 {
    // Attenuation - This seems like the most practially useful:
    // https://lisyarus.github.io/blog/graphics/2022/07/30/point-light-attenuation.html
    let s = d / range;
    let s2 = 1.0 - s * s;
    return s2 * s2 / (1.0 + falloff * s);
}

fn evaluatePBR(normal: vec3f, diffuse: vec3f, specular: vec3f, glossiness: f32, spower: f32, fnorm: f32, vvec: vec3f, lvec: vec3f, atten: f32, color: vec4f) -> vec3f {

    let hvec = normalize(lvec + vvec);
    let ndotl = max(dot(normal, lvec), 0.0);
    let ndoth = max(dot(normal, hvec), 0.0);
    let hdotl = max(dot(hvec, lvec ), 0.0);

    let fschlick = specular + (1.0 - specular) * pow(1.0 - hdotl, 5.0) * glossiness;
    let fspecular = fschlick * pow(ndoth, spower) * fnorm;

    let illum = color.rgb * color.a * ndotl * atten;

    return (diffuse + fspecular) * illum;
}

fn evaluateLighting(position: vec3f, normal: vec3f, albedo: vec4f, emissive: vec3f, metallic: f32, roughness: f32, occlusion: f32) -> vec4f {

	let black = vec3(0.04 * albedo.a);

	let diffuse = albedo.rgb * (1.0 - metallic);
	let specular = (albedo.rgb - black) * metallic + black;
	let glossiness = (1.0 - roughness) * albedo.a;
    let spower = pow(2.0, glossiness * 12.0);       // specular power
    let fnorm = (spower + 2.0) / 8.0;               // normalization factor

	let vpos = camera_uniforms.worldMatrix[3].xyz;  // viewer position
    let vvec = normalize(vpos - position);          // vector to viewer
	let ndotv = max(dot(normal, vvec), 0.0);

    let fdiffuse = lighting_uniforms.ambientLightColor.rgb * lighting_uniforms.ambientLightColor.a * diffuse;

    let mips = f32(textureNumLevels(lighting_envTexture));
    let env = textureSampleLevel(lighting_envTexture, lighting_envSampler,  reflect(-vvec, normal), roughness * mips).rgb;

	let fschlick = specular + (1.0 - specular) * pow(1.0 - ndotv, 5.0) * glossiness;
	let fspecular = env * fschlick;

	var color = (fdiffuse + fspecular) * occlusion;

	for(var i: u32 = 0; i < lighting_uniforms.numDirectionalLights; i += 1) {
	    let light = lighting_uniforms.directionalLights[i];

    	let lvec = light.direction;
    	let atten = 1.0;

	    color += evaluatePBR(normal, diffuse, specular, glossiness, spower, fnorm, vvec, lvec, atten, light.color);
	}

	for(var i: u32 = 0; i < lighting_uniforms.numPointLights; i += 1) {
	    let light = lighting_uniforms.pointLights[i];

        // vector to light
	    var lvec = light.position - position;
	    let d = length(lvec);
	    if d >= light.range {continue;}

	    // check shadow map
	    if light.castsShadow != 0 {
            let near = .1;
            let far = light.range;
            let zw = textureSampleLevel(lighting_psmTexture, lighting_psmSampler, -lvec, i, 0);
            let vz = far * near / (far + zw * (near - far));
            let dz = max(abs(lvec.x), max(abs(lvec.y), abs(lvec.z)));
            if dz > vz {continue;}
        }

	    lvec /= d;
	    let atten = pointLightAtten(d, light.range, light.falloff);

	    color += evaluatePBR(normal, diffuse, specular, glossiness, spower, fnorm, vvec, lvec, atten, light.color);
	}

	for(var i: u32 = 0; i < lighting_uniforms.numSpotLights; i += 1) {
	    let light = lighting_uniforms.spotLights[i];

        // vector to light
	    var lvec = light.position - position;
	    let d = length(lvec);
	    if d >= light.range {continue;}
	    lvec /= d;
	    let atten = pointLightAtten(d, light.range, light.falloff);

	    let cosangle = dot(lvec, light.direction);
	    if(cosangle < 0.0) {continue;}
	    let angle = acos(cosangle);
	    if(angle>light.outerConeAngle) {continue;}
	    let coneAtten = 1.0 - max((angle - light.innerConeAngle) / (light.outerConeAngle - light.innerConeAngle), 0.0);

	    color += evaluatePBR(normal, diffuse, specular, glossiness, spower, fnorm, vvec, lvec, atten * coneAtten, light.color);
	}

	return vec4f(color + emissive, albedo.a);
}

)"
