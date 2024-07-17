#include "renderpipeline.h"

#include "../material/material.h"
#include "scenebindings.h"

#define TCPP_IMPLEMENTATION
#include "tcppLibrary.hpp"

namespace sgd {

namespace {

String renderPassName(RenderPassType rpassType) {
	switch (rpassType) {
	case RenderPassType::shadow:
		return "SHADOW";
	case RenderPassType::opaque:
		return "OPAQUE";
	case RenderPassType::blend:
		return "BLEND";
	case RenderPassType::effect:
		return "EFFECT";
	default:
		SGD_ABORT();
	}
}

String blendModeName(BlendMode blendMode) {
	switch (blendMode) {
	case BlendMode::undefined:
		return "UNDEFINED";
	case BlendMode::opaque:
		return "OPAQUE";
	case BlendMode::alphaMask:
		return "ALPHA_MASK";
	case BlendMode::alphaBlend:
		return "ALPHA_BLEND";
	default:
		SGD_ABORT();
	}
}

} // namespace

wgpu::RenderPipeline getOrCreateRenderPipeline(RenderPassType rpassType,
											   BlendMode blendMode,	 //
											   DepthFunc depthFunc,	 //
											   CullMode cullMode,	 //
											   CBindGroup* bindings1, //
											   CBindGroup* bindings2, //
											   CBindGroup* bindings3) {

	static Map<uint64_t, wgpu::RenderPipeline> cache;

	auto gc = currentGC();
	auto sceneDesc = rpassType != RenderPassType::shadow ? &sceneBindingsDescriptor : &sceneShadowBindingsDescriptor;
	auto materialDesc = bindings1->descriptor();
	auto geometryDesc = bindings2->descriptor();
	auto rendererDesc = bindings3->descriptor();

	// TODO: Include colorBuffer/depthBuffer format in hash
	uint64_t hash = ((uint64_t)rpassType << 24) |		   // 3
					((uint64_t)sceneDesc->hash << 22) |	   // 2
					((uint64_t)materialDesc->hash << 18) | // 4
					((uint64_t)geometryDesc->hash << 14) | // 4
					((uint64_t)rendererDesc->hash << 10) | // 4
					((uint64_t)blendMode << 7) |		   // 3
					((uint64_t)depthFunc << 3) |		   // 4
					((uint64_t)cullMode << 0);			   // 3

	auto& pipeline = cache[hash];
	if (pipeline) return pipeline;

	// Shader module
	// Note: This generates a shader per pipeline state (ie: one for opaque, one for alpha etc), might be overkill.
	String source = sceneDesc->wgpuShaderSource +	 //
					materialDesc->wgpuShaderSource + //
					geometryDesc->wgpuShaderSource + //
					rendererDesc->wgpuShaderSource;

	{
		auto errorCallback = [](const tcpp::TErrorInfo& err) {
			SGD_LOG << "TinyCPP error:" << ErrorTypeToString(err.mType);
			SGD_ABORT();
		};

		String header;
#if SGD_OS_WINDOWS
		header += "#define OS_WINDOWS 1\n";
#elif SGD_OS_LINUX
		header += "#define OS_LINUX 1\n";
#elif SGD_OS_MACOS
		header += "#define OS_MACOS 1\n";
#elif SGD_OS_EMSCRIPTEN
		header += "#define OS_EMSCRIPTEN 1\n";
#endif
		header += "#define RENDER_PASS_" + renderPassName(rpassType) + " 1\n";
		header += "#define BLEND_MODE_" + blendModeName(blendMode) + " 1\n";

		tcpp::Lexer lexer(std::make_unique<tcpp::StringInputStream>(header + source));
		tcpp::Preprocessor preprocessor(lexer, {errorCallback});
		source = preprocessor.Process();
	}

	char label[256];
	std::strcpy(label,
				(String("(") + materialDesc->label + ',' + geometryDesc->label + ',' + rendererDesc->label + '\'').c_str());

	auto module = createShaderModule(currentGC()->wgpuDevice(), source, label);

	wgpu::RenderPipelineDescriptor pipelineDescriptor;

	// Vertex state
	auto& vertexState = pipelineDescriptor.vertex;
	vertexState.module = module;
	vertexState.entryPoint = "vertexMain";
	vertexState.bufferCount = geometryDesc->wgpuVertexBufferLayouts.size();
	vertexState.buffers = geometryDesc->wgpuVertexBufferLayouts.data();

	// Fragment state
	wgpu::FragmentState fragmentState{};
	fragmentState.module = module;
	fragmentState.entryPoint = "fragmentMain";
	pipelineDescriptor.fragment = &fragmentState;

	// Color target state
	wgpu::BlendState blendState;
	wgpu::ColorTargetState colorTargetState;
	if (rpassType == RenderPassType::shadow) {
		SGD_ASSERT(blendMode == BlendMode::opaque || blendMode == BlendMode::alphaMask);
	} else {
		switch (rpassType) {
		case RenderPassType::opaque:
		case RenderPassType::effect:
			SGD_ASSERT(blendMode == BlendMode::opaque || blendMode == BlendMode::alphaMask);
			blendState.color.srcFactor = wgpu::BlendFactor::One;
			blendState.color.dstFactor = wgpu::BlendFactor::Zero;
			break;
		case RenderPassType::blend:
			SGD_ASSERT(blendMode == BlendMode::alphaMask || blendMode == BlendMode::alphaBlend);
			blendState.color.srcFactor = wgpu::BlendFactor::One;
			blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
			break;
		default:
			SGD_ABORT();
		}
		colorTargetState.format = wgpu::TextureFormat::RGBA16Float;
		colorTargetState.blend = &blendState;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTargetState;
	}

	// Depth stencil state
	wgpu::DepthStencilState depthStencilState;
	if(rpassType == RenderPassType::effect) {
		SGD_ASSERT(blendMode == BlendMode::opaque);
	}else {
		switch (rpassType) {
		case RenderPassType::shadow:
			depthStencilState.depthWriteEnabled = true;
			break;
		case RenderPassType::opaque:
			depthStencilState.depthWriteEnabled = (depthFunc != DepthFunc::undefined);
			break;
		case RenderPassType::blend:
		case RenderPassType::effect:
			depthStencilState.depthWriteEnabled = false;
			break;
		}
		depthStencilState.format = wgpu::TextureFormat::Depth32Float;
		depthStencilState.depthCompare = (wgpu::CompareFunction)depthFunc;
		pipelineDescriptor.depthStencil = &depthStencilState;
	}

	// Pipeline layout
	wgpu::BindGroupLayout bindGroupLayouts[]{sceneDesc->wgpuBindGroupLayout(gc),	//
											 materialDesc->wgpuBindGroupLayout(gc), //
											 geometryDesc->wgpuBindGroupLayout(gc), //
											 rendererDesc->wgpuBindGroupLayout(gc)};
	wgpu::PipelineLayoutDescriptor pipelineLayoutDesc{};
	pipelineLayoutDesc.bindGroupLayoutCount = std::size(bindGroupLayouts);
	pipelineLayoutDesc.bindGroupLayouts = bindGroupLayouts;
	pipelineLayoutDesc.label = label;
	pipelineDescriptor.layout = gc->wgpuDevice().CreatePipelineLayout(&pipelineLayoutDesc);

	// Primitive state
	pipelineDescriptor.primitive.topology = geometryDesc->wgpuTopology;
	pipelineDescriptor.primitive.cullMode = (wgpu::CullMode)cullMode;

	return pipeline = gc->wgpuDevice().CreateRenderPipeline(&pipelineDescriptor);
}

wgpu::RenderPipeline getOrCreateRenderPipeline(RenderPassType rpassType, //
											   CMaterial* material,		 //
											   CBindGroup* geometry,	 //
											   CBindGroup* renderer) {

	return getOrCreateRenderPipeline(rpassType, material->blendMode(), material->depthFunc(), material->cullMode(),
									 material->bindGroup(), geometry, renderer);
}

} // namespace sgd
