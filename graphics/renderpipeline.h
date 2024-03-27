#pragma once

#include "bindgroup.h"

#include <dawn/exports.h>

namespace sgd {

SGD_SHARED(Material);

enum struct BlendMode {
	undefined,
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

wgpu::RenderPipeline getOrCreateRenderPipeline(GraphicsContext* gc, //
											   BindGroup* material, //
											   BlendMode blendMode, //
											   DepthFunc depthFunc, //
											   CullMode cullMode,	//
											   BindGroup* renderer, //
											   DrawMode drawMode);

wgpu::RenderPipeline getOrCreateRenderPipeline(GraphicsContext* gc, //
											   Material* material,	//
											   BindGroup* renderer, //
											   DrawMode drawMode);

} // namespace sgd
