R"(

// ***** Material *****

fn evaluateMaterial(position: vec3f, tanMatrix: mat3x3f, texCoords: vec3f, color: vec4f) -> vec4f {

	return evaluateLighting(position, normalize(tanMatrix[2]), color, Vec3f(0), 0, 1, 1);
}

)"
