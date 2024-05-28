#pragma once

#include "bindgroup.h"

#include <dawn/exports.h>

namespace sgd {

SGD_SHARED(Material);

enum struct BlendMode {
	undefined,
	opaque,
	alphaMask,
	alphaBlend,
	additive,
	multiply,
};

// wgpu::CompareFunction func
enum struct DepthFunc { undefined, never, less, equal, lessEqual, greater, notEqual, greaterEqual, always };

// wgpu::CullMode cullMode
enum struct CullMode { undefined, none, front, back };

// wgpu::PrimitiveTopology
enum struct DrawMode { undefined, pointList, lineList, lineStrip, triangleList, triangleStrip };

// BlendMode::undefined == no color buffer
// BlendMode::opaque && DepthFunc::undefined == no depth write (ie: clear pass)
wgpu::RenderPipeline getOrCreateRenderPipeline(GraphicsContext* gc, //
											   CBindGroup* material, //
											   BlendMode blendMode, //
											   DepthFunc depthFunc, //
											   CullMode cullMode,	//
											   CBindGroup* renderer, //
											   DrawMode drawMode);

wgpu::RenderPipeline getOrCreateRenderPipeline(GraphicsContext* gc, //
											   CMaterial* material,	//
											   CBindGroup* renderer, //
											   DrawMode drawMode);

wgpu::RenderPipeline getOrCreateShadowPipeline(GraphicsContext* gc, //
											   CBindGroup* renderer, //
											   DrawMode drawMode);

} // namespace sgd
