R"(

// ***** FontMaterial *****

struct FontMaterialUniforms {
    atlasColor: vec4f,
}

@group(1) @binding(0) var<uniform> material_uniforms: FontMaterialUniforms;
@group(1) @binding(1) var material_atlasTexture: texture_2d<f32>;
@group(1) @binding(2) var material_atlasSampler: sampler;

fn evaluateMaterial(position: vec3f, tanMatrix: mat3x3f, texCoords: vec3f, color: vec4f) -> vec4f {

    let r = textureSample(material_atlasTexture, material_atlasSampler, texCoords.xy).r;

    return color * r;// * r;
}

)"
