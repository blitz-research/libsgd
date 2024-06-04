#pragma once

#include <dawn/exports.h>

namespace sgd {

SGD_SHARED(GraphicsContext);
SGD_SHARED(BindGroup);
SGD_SHARED(Material);

enum struct RenderPassType { shadow, opaque, blend };
constexpr int renderPassTypeCount = 3;

enum struct BlendMode { undefined, opaque, alphaMask, alphaBlend };

enum struct DepthFunc { undefined, never, less, equal, lessEqual, greater, notEqual, greaterEqual, always };

enum struct CullMode { undefined, none, front, back };

enum struct DrawMode { undefined, pointList, lineList, lineStrip, triangleList, triangleStrip };

wgpu::RenderPipeline getOrCreateRenderPipeline(GraphicsContext* gc,		 //
											   RenderPassType rpassType, //
											   CBindGroup* material,	 //
											   BlendMode blendMode,		 //
											   DepthFunc depthFunc,		 //
											   CullMode cullMode,		 //
											   CBindGroup* renderer,	 //
											   DrawMode drawMode);

wgpu::RenderPipeline getOrCreateRenderPipeline(GraphicsContext* gc,		 //
											   RenderPassType rpassType, //
											   CMaterial* material,		 //
											   CBindGroup* renderer,	 //
											   DrawMode drawMode);

} // namespace sgd
