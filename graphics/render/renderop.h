#pragma once

#include <dawn/exports.h>

namespace sgd {

struct RenderOp {
	wgpu::Buffer vbuffer;
	wgpu::Buffer ibuffer;
	wgpu::BindGroup material;
	wgpu::BindGroup geometry;
	wgpu::BindGroup renderer;
	wgpu::RenderPipeline pipeline;
	uint32_t elementCount{};
	uint32_t instanceCount{};
	uint32_t firstElement{};
	uint32_t firstInstance{};

	RenderOp() = default;

	RenderOp(							 //
		wgpu::Buffer vbuffer,			 //
		wgpu::Buffer ibuffer,			 //
		wgpu::BindGroup material,		 //
		wgpu::BindGroup geometry,		 //
		wgpu::BindGroup renderer,		 //
		wgpu::RenderPipeline pipeline,	 //
		uint32_t elementCount,			 //
		uint32_t instanceCount,			 //
		uint32_t firstElement,			 //
		uint32_t firstInstance)			 //
		: vbuffer(std::move(vbuffer)),	 //
		  ibuffer(std::move(ibuffer)),	 //
		  material(std::move(material)), //
		  geometry(std::move(geometry)), //
		  renderer(std::move(renderer)), //
		  pipeline(std::move(pipeline)), //
		  elementCount(elementCount),	 //
		  instanceCount(instanceCount),	 //
		  firstElement(firstElement),	 //
		  firstInstance(firstInstance) {
	}
};
using CRenderOp = const RenderOp&;

} // namespace sgd
