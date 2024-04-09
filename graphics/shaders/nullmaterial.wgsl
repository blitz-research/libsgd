R"(

// ***** Material *****

fn evaluateMaterial(position: vec3f, tanMatrix: mat3x3f, texCoords: vec2f, color: vec4f) -> vec4f {

	let litColor = evaluateLighting(position, normalize(tanMatrix[2]), color.rgb, 0, 1, 1);

	return vec4f(litColor, color.a);
}

)"
