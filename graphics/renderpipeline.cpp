#include "renderpipeline.h"

#include "material.h"
#include "shadowmaterial.h"
#include "scenebindings.h"

namespace sgd {

wgpu::RenderPipeline getOrCreateRenderPipeline(GraphicsContext* gc, //
											   CBindGroup* material, //
											   BlendMode blendMode, //
											   DepthFunc depthFunc, //
											   CullMode cullMode,	//
											   CBindGroup* renderer, //
											   DrawMode drawMode) {

	static Map<uint64_t, wgpu::RenderPipeline> cache;

	auto* sceneBindingsDesc = &sceneBindingsDescriptor;

	if (!material) material = emptyBindGroup(1);

	// TODO: Include colorBuffer/depthBuffer format in hash
	uint64_t hash = ((uint64_t)sceneBindingsDesc->hash << 56) |		 //
					((uint64_t)material->descriptor()->hash << 48) | //
					((uint64_t)renderer->descriptor()->hash << 40) | //
					((uint64_t)blendMode << 32) |					 //
					((uint64_t)depthFunc << 24) |					 //
					((uint64_t)cullMode << 16) |					 //
					((uint64_t)drawMode << 8);

	auto& pipeline = cache[hash];
	if (pipeline) return pipeline;

	// Cheating!
	material->validate(gc);
	renderer->validate(gc);

	// Shader module
	// Note: This generates a shader per pipeline state (ie: one for opaque, one for alpha etc), might be overkill.
	String source = sceneBindingsDesc->wgpuShaderSource +	   //
					material->descriptor()->wgpuShaderSource + //
					renderer->descriptor()->wgpuShaderSource;

	auto module = createShaderModule(gc->wgpuDevice(), source);

	wgpu::RenderPipelineDescriptor pipelineDescriptor;

	// Fragment state
	wgpu::FragmentState fragmentState{};
	fragmentState.module = module;
	fragmentState.entryPoint = "fragmentMain";
	pipelineDescriptor.fragment = &fragmentState;

	// Color target state
	wgpu::BlendState blendState;
	wgpu::ColorTargetState colorTargetState;
	if (blendMode != BlendMode::undefined) {
		switch (blendMode) {
		case BlendMode::undefined:
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
		colorTargetState.format = gc->colorBuffer()->wgpuTexture().GetFormat();
		colorTargetState.blend = &blendState;

		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTargetState;
	}

	// Vertex state
	auto& vertexState = pipelineDescriptor.vertex;
	vertexState.module = module;
	vertexState.entryPoint = "vertexMain";
	vertexState.bufferCount = renderer->descriptor()->wgpuVertexBufferLayouts.size();
	vertexState.buffers = renderer->descriptor()->wgpuVertexBufferLayouts.data();

	// Depth stencil state
	wgpu::DepthStencilState depthStencilState;
	depthStencilState.format = gc->depthBuffer()->wgpuTexture().GetFormat();
	depthStencilState.depthCompare = (wgpu::CompareFunction)depthFunc;
	switch (blendMode) {
	case BlendMode::undefined:	// for shadows
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
	wgpu::BindGroupLayout bindGroupLayouts[]{sceneBindingsDesc->wgpuBindGroupLayout(gc),	  //
											 material->descriptor()->wgpuBindGroupLayout(gc), //
											 renderer->descriptor()->wgpuBindGroupLayout(gc)};

	wgpu::PipelineLayoutDescriptor pipelineLayoutDesc{};
	pipelineLayoutDesc.bindGroupLayoutCount = std::size(bindGroupLayouts);
	pipelineLayoutDesc.bindGroupLayouts = bindGroupLayouts;
	pipelineLayoutDesc.label = renderer->descriptor()->label;

	pipelineDescriptor.layout = gc->wgpuDevice().CreatePipelineLayout(&pipelineLayoutDesc);

	// Primitive state
	pipelineDescriptor.primitive.topology = (wgpu::PrimitiveTopology)drawMode;
	pipelineDescriptor.primitive.cullMode = (wgpu::CullMode)cullMode;

	return pipeline = gc->wgpuDevice().CreateRenderPipeline(&pipelineDescriptor);
}

wgpu::RenderPipeline getOrCreateRenderPipeline(GraphicsContext* gc, //
											   CMaterial* material,	//
											   CBindGroup* renderer, //
											   DrawMode drawMode) {

	return getOrCreateRenderPipeline(gc, material->bindGroup(), material->blendMode(), material->depthFunc(),
									 material->cullMode(), renderer, drawMode);
}

wgpu::RenderPipeline getOrCreateShadowPipeline(GraphicsContext* gc, //
											   CBindGroup* renderer, //
											   DrawMode drawMode) {

//	return getOrCreateRenderPipeline(gc, shadowBindGroup(), BlendMode::undefined, DepthFunc::lessEqual, CullMode::back, renderer,
//									 drawMode);
	return getOrCreateRenderPipeline(gc, shadowBindGroup(), BlendMode::undefined, DepthFunc::less, CullMode::front, renderer,
									 drawMode);
}

} // namespace sgd
