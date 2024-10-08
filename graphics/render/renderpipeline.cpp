#include "renderpipeline.h"

#include "../material/material.h"
#include "scenebindings.h"

#define TCPP_IMPLEMENTATION
#include "tcppLibrary.hpp"

namespace sgd {

String toString(RenderPassType rpassType) {
	static const Map<RenderPassType, String> types{
		{RenderPassType::shadow, "SHADOW"},
		{RenderPassType::opaque, "OPAQUE"},
		{RenderPassType::blend, "BLEND"},
	};
	return types.find(rpassType)->second;
}

String toString(BlendMode blendMode) {
	static const Map<BlendMode, String> modes{
		{BlendMode::undefined, "UNDEFINED"},
		{BlendMode::opaque, "OPAQUE"},
		{BlendMode::alphaMask, "ALPHA_MASK"},
		{BlendMode::alphaBlend, "ALPHA_BLEND"},
	};
	return modes.find(blendMode)->second;
}

String toString(DepthFunc depthFunc) {
	static const Map<DepthFunc, String> funcs{
		{DepthFunc::undefined, "UNDEFINED"},
		{DepthFunc::never, "NEVER"},
		{DepthFunc::less, "LESS"},
		{DepthFunc::equal, "EQUAL"},
		{DepthFunc::lessEqual, "LESS_EQUAL"},
		{DepthFunc::greater, "GREATER"},
		{DepthFunc::notEqual, "NOT_EQUAL"},
		{DepthFunc::greaterEqual, "GREATER_EQUAL"},
		{DepthFunc::always, "ALWAYS"},
	};
	return funcs.find(depthFunc)->second;
}

String toString(CullMode cullMode) {
	static const Map<CullMode, String> modes{
		{CullMode::undefined, "UNDEFINED"},
		{CullMode::none, "NONE"},
		{CullMode::front, "FRONT"},
		{CullMode::back, "BACK"},
	};
	return modes.find(cullMode)->second;
}

wgpu::RenderPipeline getOrCreateRenderPipeline(RenderPassType rpassType,
											   BlendMode blendMode,	  //
											   DepthFunc depthFunc,	  //
											   CullMode cullMode,	  //
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
		header += "#define RENDER_PASS_" + toString(rpassType) + " 1\n";
		header += "#define BLEND_MODE_" + toString(blendMode) + " 1\n";

		tcpp::Lexer lexer(std::make_unique<tcpp::StringInputStream>(header + source));
		tcpp::Preprocessor preprocessor(lexer, {errorCallback});
		source = preprocessor.Process();
	}

	char label[256];
	std::strcpy(label,
				(String("(") + materialDesc->label + ',' + geometryDesc->label + ',' + rendererDesc->label + '\'').c_str());

	auto module = createShaderModule(currentGC()->wgpuDevice(), source, label);

	wgpu::RenderPipelineDescriptor pipelineDescriptor;

	// Vertex state - should probably always be in renderer?
	auto& vertexBufferLayouts = //
		!geometryDesc->wgpuVertexBufferLayouts.empty() ? geometryDesc->wgpuVertexBufferLayouts
													   : rendererDesc->wgpuVertexBufferLayouts;

	auto& vertexState = pipelineDescriptor.vertex;
	vertexState.module = module;
	vertexState.entryPoint = "vertexMain";
	vertexState.bufferCount = vertexBufferLayouts.size();
	vertexState.buffers = vertexBufferLayouts.data();

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
		switch (blendMode) {
		case BlendMode::undefined:
			SGD_ABORT();
		case BlendMode::opaque:
		case BlendMode::alphaMask:
			blendState.color.srcFactor = wgpu::BlendFactor::One;
			blendState.color.dstFactor = wgpu::BlendFactor::Zero;
			break;
		case BlendMode::alphaBlend:
			blendState.color.srcFactor = wgpu::BlendFactor::One;
			blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
			break;
		}
		colorTargetState.format = wgpu::TextureFormat::RGBA16Float;
		colorTargetState.blend = &blendState;
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTargetState;
	}

	// Depth stencil state
	wgpu::DepthStencilState depthStencilState;
	if (depthFunc != DepthFunc::undefined) { // hack for no depth buffer
		depthStencilState.format = wgpu::TextureFormat::Depth32Float;
		depthStencilState.depthCompare = (wgpu::CompareFunction)depthFunc;
		switch (rpassType) {
		case RenderPassType::shadow:
		case RenderPassType::opaque:
			depthStencilState.depthWriteEnabled = depthFunc != DepthFunc::always; // hack for no depth write
			break;
		case RenderPassType::blend:
			depthStencilState.depthWriteEnabled = false;
			break;
		}
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
	// topology - should probably always be in renderer?
	auto topology = //
		geometryDesc->wgpuTopology != wgpu::PrimitiveTopology::Undefined ? geometryDesc->wgpuTopology
																		 : rendererDesc->wgpuTopology;
	SGD_ASSERT(topology != wgpu::PrimitiveTopology::Undefined);

	pipelineDescriptor.primitive.topology = topology;
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
