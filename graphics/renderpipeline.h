#pragma once

#include <dawn/exports.h>

namespace sgd {

SGD_SHARED(GraphicsContext);
SGD_SHARED(BindGroup);
SGD_SHARED(Material);

enum struct RenderPassType { shadow, opaque, blend, effect };
constexpr int renderPassTypeCount = 4;

enum struct BlendMode { undefined, opaque, alphaMask, alphaBlend };

enum struct DepthFunc { undefined, never, less, equal, lessEqual, greater, notEqual, greaterEqual, always };

enum struct CullMode { undefined, none, front, back };

wgpu::RenderPipeline getOrCreateRenderPipeline(GraphicsContext* gc,		 //
											   RenderPassType rpassType, //
											   CBindGroup* material,	 //
											   BlendMode blendMode,		 //
											   DepthFunc depthFunc,		 //
											   CullMode cullMode,		 //
											   CBindGroup* renderer);

wgpu::RenderPipeline getOrCreateRenderPipeline(GraphicsContext* gc,		 //
											   RenderPassType rpassType, //
											   CMaterial* material,		 //
											   CBindGroup* renderer);

} // namespace sgd
