R"(

// If the uniformity analysis is wrong because it can't prove uniformity, but you want to assert that the control flow is actually uniform,
// in a fragment shader you can use:
// @diagnostic(off, derivative_uniformity)

// ***** std header *****

const pi = 3.14159265359;
const twoPi = pi * 2.0;
const halfPi = pi * 0.5;
const degreesToRadians = 0.01745329252;
const radiansToDegrees = 57.295779513;

// Weird, but the only thing that seemed to work on Metal...
fn isNan(tz:f32) ->bool {
    if tz > 0.0 {
        if tz <= 0.0 {return true;}
    }else if tz < 0.0 {
        if tz >= 0.0 {return true;}
    }else if tz == 0.0 {
        if tz != 0.0 {return true;}
    }else{
        return true;
    }
    return false;
}

// ***** Scene *****

// Note: *MUST* stay in sync with scenebindgroup.h
//
const maxDirectionalLights: u32 = 4;
const maxPointLights: u32 = 32;
const maxSpotLights: u32 = 16;

const configUniformsBinding = 0;
const cameraUniformsBinding = 1;
const lightingUniformsBinding = 2;
const envTextureCubeBinding = 3;
const envTexture2DBinding = 4;
const envSamplerBinding = 5;
const csmTextureBinding = 6;
const csmSamplerBinding = 7;
const csmMatricesBinding = 8;
const psmTextureBinding = 9;
const psmSamplerBinding = 10;
const ssmTextureBinding = 11;
const ssmSamplerBinding = 12;
const ssmMatricesBinding = 13;

struct ConfigUniforms {

	// CSM settings
	csmSplitDistances: vec4f,
	csmTextureSize: u32,
	maxCSMLights: u32,
	csmClipRange: f32,
	csmDepthBias: f32,
	
	// PSM Settings
	psmTextureSize: u32,
    maxPSMLights: u32,
	psmClipNear: f32,
	psmDepthBias: f32,
	
	// SSM Settings
	ssmTextureSize: u32,
	maxSSMLights: u32,
	ssmClipNear: f32,
    ssmDepthBias: f32,
};

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
    worldMatrix: mat4x4f,
    color: vec4f,
}

struct PointLight {
    position: vec4f,
 	color: vec4f,
	range: f32,
	falloff: f32,
}

struct SpotLight {
    worldMatrix: mat4x4f,
 	color: vec4f,
	range: f32,
	falloff: f32,
	innerConeAngle: f32,
	outerConeAngle: f32,
}

struct LightingUniforms {
	ambientLightColor: vec4f,

	envTextureType: u32,    // 0 = cube, 1 = 2D
	
	numDirectionalLights: u32,
	numPointLights: u32,
	numSpotLights: u32,
	
	numCSMLights: u32,
	numPSMLights: u32,
	numSSMLights: u32,
	
	directionalLights: array<DirectionalLight, maxDirectionalLights>,
	pointLights: array<PointLight, maxPointLights>,
	spotLights: array<SpotLight, maxSpotLights>,
}

@group(0) @binding(configUniformsBinding) var<uniform> scene_config: ConfigUniforms;
@group(0) @binding(cameraUniformsBinding) var<uniform> scene_camera: CameraUniforms;

#if !RENDER_PASS_SHADOWS

@group(0) @binding(lightingUniformsBinding) var<uniform> scene_lighting: LightingUniforms;
@group(0) @binding(envTextureCubeBinding) var scene_envTextureCube: texture_cube<f32>;
@group(0) @binding(envTexture2DBinding) var scene_envTexture2D: texture_2d<f32>;
@group(0) @binding(envSamplerBinding) var scene_envSampler: sampler;
@group(0) @binding(csmTextureBinding) var scene_csmTexture: texture_depth_2d_array;
@group(0) @binding(csmSamplerBinding) var scene_csmSampler: sampler_comparison;
@group(0) @binding(csmMatricesBinding) var<storage> scene_csmMatrices: array<mat4x4f>;
@group(0) @binding(psmTextureBinding) var scene_psmTexture: texture_depth_cube_array;
@group(0) @binding(psmSamplerBinding) var scene_psmSampler: sampler_comparison;
@group(0) @binding(ssmTextureBinding) var scene_ssmTexture: texture_depth_2d_array;
@group(0) @binding(ssmSamplerBinding) var scene_ssmSampler: sampler_comparison;
@group(0) @binding(ssmMatricesBinding) var<storage> scene_ssmMatrices: array<mat4x4f>;

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

	let vpos = scene_camera.worldMatrix[3].xyz;     // viewer position
    let vvec = normalize(vpos - position);          // vector to viewer
	let ndotv = max(dot(normal, vvec), 0.0);

    var env:vec3f;
    {
        let rvec = reflect(-vvec, normal);
        let mipLevel  = roughness * f32(textureNumLevels(scene_envTextureCube));
        if scene_lighting.envTextureType == 1 {
            //
            env = textureSampleLevel(scene_envTextureCube, scene_envSampler,  rvec, mipLevel).rgb;
            //
        } else {
            //
            let u = atan2(rvec.x, rvec.z) / pi * 0.5 + 0.5;
            let v = -atan2(rvec.y, length(rvec.xz)) / pi + 0.5;
            env = textureSampleLevel(scene_envTexture2D, scene_envSampler, vec2f(u, v), mipLevel).rgb;
        }
    }

    let fdiffuse = scene_lighting.ambientLightColor.rgb * scene_lighting.ambientLightColor.a * diffuse;
	let fschlick = specular + (1.0 - specular) * pow(1.0 - ndotv, 5.0) * glossiness;
	let fspecular = fschlick * env;

	var color = (fdiffuse + fspecular) * occlusion;

    // ***** Directional lights with shadows *****
	for(var i: u32 = 0; i < scene_lighting.numCSMLights; i += 1) {

	    let light = scene_lighting.directionalLights[i];

	    // Shadow
        let vpos = (scene_camera.viewMatrix * vec4f(position, 1.0)).xyz;

        if vpos.z < 0.0 || vpos.z >= scene_config.csmSplitDistances.w {
            return vec4(1,1,0,1);
        }

        var split = i * 4;
        if vpos.z >= scene_config.csmSplitDistances.x {
            if vpos.z < scene_config.csmSplitDistances.y {
                split += 1;
            } else if vpos.z < scene_config.csmSplitDistances.z {
                split += 2;
            } else {
                split += 3;
            }
        }
        let wpos = scene_csmMatrices[split] * vec4f(position, 1);
        let spos = wpos.xyz / wpos.w;
        let tcoords = spos.xy * vec2f(0.5, -0.5) + 0.5;
        var atten = textureSampleCompareLevel(scene_csmTexture, scene_csmSampler, tcoords, split, spos.z - scene_config.csmDepthBias);
        if atten == 0.0 {continue;}

    	let lvec = -light.worldMatrix[2].xyz;

	    color += evaluatePBR(normal, diffuse, specular, glossiness, spower, fnorm, vvec, lvec, atten, light.color);
	}

	// ***** Directional lights without shadows *****
	for(var i: u32 = scene_lighting.numCSMLights; i < scene_lighting.numDirectionalLights; i += 1) {

	    let light = scene_lighting.directionalLights[i];

    	let lvec = -light.worldMatrix[2].xyz;

	    color += evaluatePBR(normal, diffuse, specular, glossiness, spower, fnorm, vvec, lvec, 1.0, light.color);
	}

    // ***** Point lights with shadows *****
	for(var i: u32 = 0; i < scene_lighting.numPSMLights; i += 1) {
	    let light = scene_lighting.pointLights[i];

        var lvec = light.position.xyz - position;
	    let d = length(lvec);
	    if d >= light.range {continue;}

        // Shadow
        let near = scene_config.psmClipNear;
        let far = light.range;
        let z = max(abs(lvec.x), max(abs(lvec.y), abs(lvec.z)));
        let zw = (far * (near - z)) / (z * (near - far));
        var atten = textureSampleCompareLevel(scene_psmTexture, scene_psmSampler, -lvec, i, zw - scene_config.psmDepthBias);
        if atten == 0.0 {continue;}

	    lvec /= d;
	    atten *= pointLightAtten(d, light.range, light.falloff);

	    color += evaluatePBR(normal, diffuse, specular, glossiness, spower, fnorm, vvec, lvec, atten, light.color);
	}

	// ***** Point lights without shadows *****
	for(var i: u32 = scene_lighting.numPSMLights; i < scene_lighting.numPointLights; i += 1) {

	    let light = scene_lighting.pointLights[i];

	    var lvec = light.position.xyz - position;
	    let d = length(lvec);
	    if d >= light.range {continue;}

	    lvec /= d;
	    let atten = pointLightAtten(d, light.range, light.falloff);

	    color += evaluatePBR(normal, diffuse, specular, glossiness, spower, fnorm, vvec, lvec, atten, light.color);
	}

	// ***** Spot lights with shadows *****
	for(var i: u32 = 0; i < scene_lighting.numSSMLights; i += 1) {

	    let light = scene_lighting.spotLights[i];

	    var lvec = light.worldMatrix[3].xyz - position;
	    let d = length(lvec);
	    if d >= light.range {continue;}

        // Shadow
        let wpos = scene_ssmMatrices[i] * vec4f(position, 1);
        let spos = wpos.xyz / wpos.w;
        let tcoords = spos.xy * vec2f(0.5, -0.5) + 0.5;
        var atten = textureSampleCompareLevel(scene_ssmTexture, scene_ssmSampler, tcoords, i, spos.z - scene_config.ssmDepthBias);
        if(atten == 0.0) {continue;}

	    lvec /= d;
	    let cosangle = dot(-lvec, light.worldMatrix[2].xyz);
	    if cosangle < 0.0 {continue;}
	    let angle = acos(cosangle);
	    if angle > light.outerConeAngle {continue;}
	    atten *= 1.0 - max((angle - light.innerConeAngle) / (light.outerConeAngle - light.innerConeAngle), 0.0);
	    atten *= pointLightAtten(d, light.range, light.falloff);

	    color += evaluatePBR(normal, diffuse, specular, glossiness, spower, fnorm, vvec, lvec, atten, light.color);
	}

	// ***** Spot lights without shadows *****
	for(var i: u32 = scene_lighting.numSSMLights; i < scene_lighting.numSpotLights; i += 1) {

	    let light = scene_lighting.spotLights[i];

	    var lvec = light.worldMatrix[3].xyz - position;
	    let d = length(lvec);
	    if d >= light.range {continue;}

	    lvec /= d;
	    let cosangle = dot(-lvec, light.worldMatrix[2].xyz);
	    if cosangle < 0.0 {continue;}
	    let angle = acos(cosangle);
	    if angle > light.outerConeAngle {continue;}
	    var atten = 1.0 - max((angle - light.innerConeAngle) / (light.outerConeAngle - light.innerConeAngle), 0.0);
	    atten *= pointLightAtten(d, light.range, light.falloff);

	    color += evaluatePBR(normal, diffuse, specular, glossiness, spower, fnorm, vvec, lvec, atten, light.color);
	}

	return vec4f(color + emissive, albedo.a);
#if OS_MACOS
	return vec4f(color + emissive, albedo.a);
#endif
}

#endif

)"
