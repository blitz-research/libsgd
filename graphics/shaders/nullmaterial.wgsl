R"(

// ***** Material *****

fn evaluateMaterial(position: vec3f, normal: vec3f, texCoords: vec2f, color: vec4f) -> MaterialResult {

    var result: MaterialResult;

	result.albedo = color;
	result.emissive = vec3f(0.0);
	result.normal = vec3f(0.0, 0.0, -1.0);
	result.metallic = 0.0;
	result.roughness = 1.0;
	result.occlusion = 1.0;

	return result;
}

)"
