#pragma once

#include "bindgroup.h"
#include "buffer.h"
#include "graphics.h"

namespace sgd {

struct RenderOp {
	wgpu::Buffer vertexBuffer;
	wgpu::Buffer instanceBuffer;
	wgpu::Buffer indexBuffer;
	wgpu::BindGroup materialBindings;
	wgpu::BindGroup rendererBindings;
	wgpu::RenderPipeline pipeline;
	uint32_t elementCount{};
	uint32_t instanceCount{};
	uint32_t firstElement{};

	RenderOp() = default;

	// clang-format off
	RenderOp(
		wgpu::Buffer vertexBuffer,
		wgpu::Buffer instanceBuffer,
		wgpu::Buffer indexBuffer,
		wgpu::BindGroup materialBindings,
		wgpu::BindGroup rendererBindings,
		wgpu::RenderPipeline pipeline,
		uint32_t elementCount,
		uint32_t instanceCount,
		uint32_t firstElement):
		vertexBuffer(std::move(vertexBuffer)),
		instanceBuffer(std::move(instanceBuffer)),
		indexBuffer(std::move(indexBuffer)),
		materialBindings(std::move(materialBindings)),
		rendererBindings(std::move(rendererBindings)),
		pipeline(std::move(pipeline)),
		elementCount(elementCount),
		instanceCount(instanceCount),
		firstElement(firstElement){}

	RenderOp(
		const Buffer* vertexBuffer,
		const Buffer* instanceBuffer,
		const Buffer* indexBuffer,
		const BindGroup* materialBindings,
		const BindGroup* rendererBindings,
		wgpu::RenderPipeline pipeline,
		uint32_t elementCount,
		uint32_t instanceCount,
		uint32_t firstElement):
		vertexBuffer(vertexBuffer ? vertexBuffer->wgpuBuffer() : nullptr),
		instanceBuffer(instanceBuffer ? instanceBuffer->wgpuBuffer() : nullptr),
		indexBuffer(indexBuffer ? indexBuffer->wgpuBuffer() : nullptr),
		materialBindings(materialBindings ? materialBindings->wgpuBindGroup() : nullptr),
		rendererBindings(rendererBindings->wgpuBindGroup()),
		pipeline(std::move(pipeline)),
		elementCount(elementCount),
		instanceCount(instanceCount),
		firstElement(firstElement){}
	// clang-format on
};

} // namespace sgd
