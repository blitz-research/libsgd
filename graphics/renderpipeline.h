#pragma once

#include "bindgroup.h"

#include <dawn/exports.h>

namespace sgd {

enum struct BlendMode {
	clear,
	opaque,
	alpha,
	additive,
	multiply,
};

// wgpu::CompareFunction func
enum struct DepthFunc { undefined, never, less, equal, lessEqual, greater, notEqual, greaterEqual, always };

// wgpu::CullMode cullMode
enum struct CullMode { undefined, none, front, back };

// wgpu::PrimitiveTopology
enum struct DrawMode { undefined, pointList, lineList, lineStrip, triangleList, triangleStrip };

wgpu::RenderPipeline getRenderPipeline(GraphicsContext* gc,							 //
									   BindGroup* bindGroup1, BindGroup* bindGroup2, //
									   BlendMode blendMode, DepthFunc depthFunc, CullMode cullMode, DrawMode drawMode);

} // namespace sgd
