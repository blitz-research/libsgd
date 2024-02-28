#include "renderpipeline.h"

namespace sgd {

wgpu::RenderPipeline getRenderPipeline(GraphicsContext* gc, BindGroup* bindGroup1, BindGroup* bindGroup2, BlendMode blendMode,
									   DepthFunc depthFunc, CullMode cullMode, DrawMode drawMode) {

	static Map<uint64_t, wgpu::RenderPipeline> cache;

	auto bindGroup0 = gc->bindGroup0();

	// TODO: Include colorBuffer/depthBuffer format in hash
	uint64_t hash = ((uint64_t)bindGroup0->descriptor()->hash << 56) | //
					((uint64_t)bindGroup1->descriptor()->hash << 48) | //
					((uint64_t)bindGroup2->descriptor()->hash << 40) | //
					((uint64_t)blendMode << 32) |					   //
					((uint64_t)depthFunc << 24) |					   //
					((uint64_t)cullMode << 16) |					   //
					((uint64_t)drawMode << 8);

	auto& pipeline = cache[hash];
	if (pipeline) return pipeline;

	bindGroup0->validate(gc);
	bindGroup1->validate(gc);
	bindGroup2->validate(gc);

	wgpu::RenderPipelineDescriptor pipelineDescriptor;

	// Blend state
	wgpu::BlendState blendState;
	switch (blendMode) {
	case BlendMode::opaque:
		blendState.color.srcFactor = wgpu::BlendFactor::One;
		blendState.color.dstFactor = wgpu::BlendFactor::Zero;
		break;
	case BlendMode::alpha:
		blendState.color.srcFactor = wgpu::BlendFactor::One;
		blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
		break;
	case BlendMode::additive:
		blendState.color.srcFactor = wgpu::BlendFactor::One;
		blendState.color.dstFactor = wgpu::BlendFactor::One;
		break;
	case BlendMode::multiply:
		blendState.color.srcFactor = wgpu::BlendFactor::Dst;
		blendState.color.dstFactor = wgpu::BlendFactor::Zero;
		break;
	}

	// Color target state
	wgpu::ColorTargetState colorTargetState;
	colorTargetState.format = gc->colorBuffer()->wgpuTexture().GetFormat();
	colorTargetState.blend = &blendState;

	// Shader module
	// Note: This generates a shader per pipeline state (ie: one for opaque, one for alpha etc), might be overkill.
	String source = bindGroup0->descriptor()->wgpuShaderSource + //
					bindGroup1->descriptor()->wgpuShaderSource + //
					bindGroup2->descriptor()->wgpuShaderSource;

	auto module = createShaderModule(gc->wgpuDevice(), source);

	// Fragment state
	wgpu::FragmentState fragmentState;
	fragmentState.targetCount = 1;
	fragmentState.targets = &colorTargetState;
	fragmentState.module = module;
	fragmentState.entryPoint = "fragmentMain";
	pipelineDescriptor.fragment = &fragmentState;

	// Vertex state
	auto& vertexState = pipelineDescriptor.vertex;
	vertexState.module = module;
	vertexState.entryPoint = "vertexMain";
	vertexState.bufferCount = bindGroup2->descriptor()->wgpuVertexBufferLayouts.size();
	vertexState.buffers = bindGroup2->descriptor()->wgpuVertexBufferLayouts.data();

	// Depth stencil state
	wgpu::DepthStencilState depthStencilState;
	depthStencilState.format = gc->depthBuffer()->wgpuTexture().GetFormat();
	depthStencilState.depthCompare = (wgpu::CompareFunction)depthFunc;
	switch (blendMode) {
	case BlendMode::opaque:
		depthStencilState.depthWriteEnabled = (depthFunc != DepthFunc::undefined);
		break;
	case BlendMode::alpha:
	case BlendMode::additive:
	case BlendMode::multiply:
		depthStencilState.depthWriteEnabled = false;
		break;
	}
	pipelineDescriptor.depthStencil = &depthStencilState;

	// Pipeline layout
	wgpu::BindGroupLayout bindGroupLayouts[]{bindGroup0->descriptor()->wgpuBindGroupLayout(gc), //
											 bindGroup1->descriptor()->wgpuBindGroupLayout(gc), //
											 bindGroup2->descriptor()->wgpuBindGroupLayout(gc)};

	wgpu::PipelineLayoutDescriptor pipelineLayoutDesc{};
	pipelineLayoutDesc.bindGroupLayoutCount = std::size(bindGroupLayouts);
	pipelineLayoutDesc.bindGroupLayouts = bindGroupLayouts;

	pipelineDescriptor.layout = gc->wgpuDevice().CreatePipelineLayout(&pipelineLayoutDesc);

	// Primitive state
	pipelineDescriptor.primitive.topology = (wgpu::PrimitiveTopology)drawMode;
	pipelineDescriptor.primitive.cullMode = (wgpu::CullMode)cullMode;

	return pipeline = gc->wgpuDevice().CreateRenderPipeline(&pipelineDescriptor);
}

} // namespace sgd
