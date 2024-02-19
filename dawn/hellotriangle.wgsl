R"(

@vertex fn vertexMain(@builtin(vertex_index) vertexId: u32) -> @builtin(position) vec4f {

    const pos = array<vec2f, 3>(vec2f( 0.0,  0.5), vec2f(-0.5, -0.5), vec2f( 0.5, -0.5));

    return vec4f(pos[vertexId], 0.0, 1.0);
}

@fragment fn fragmentMain() -> @location(0) vec4<f32> {
    return vec4f(1.0, 0.0, 0.0, 1.0);
}

)"