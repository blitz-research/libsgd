#include "renderpipeline.h"

#include "material.h"
#include "scenebindings.h"
#include "shadowmaterial.h"

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

wgpu::RenderPipeline getOrCreateRenderPipeline(GraphicsContext* gc,		 //
											   RenderPassType rpassType, //
											   CBindGroup* material,	 //
											   BlendMode blendMode,		 //
											   DepthFunc depthFunc,		 //
											   CullMode cullMode,		 //
											   CBindGroup* renderer,	 //
											   DrawMode drawMode) {

	static Map<uint64_t, wgpu::RenderPipeline> cache;

	auto sceneBindingsDesc = &sceneBindingsDescriptor;

	if (!material) material = emptyBindGroup(1);

	// TODO: Include colorBuffer/depthBuffer format in hash
	uint64_t hash = ((uint64_t)sceneBindingsDesc->hash << 32) |		 // 2
					((uint64_t)material->descriptor()->hash << 26) | // 6
					((uint64_t)renderer->descriptor()->hash << 20) | // 6
					((uint64_t)rpassType << 16) |					 // 4
					((uint64_t)blendMode << 12) |					 // 4
					((uint64_t)depthFunc << 8) |					 // 4
					((uint64_t)cullMode << 4) |						 // 4
					((uint64_t)drawMode << 0);						 // 4

	auto& pipeline = cache[hash];
	if (pipeline) return pipeline;

	// Cheating!
	//	material->validate(gc);
	//	renderer->validate(gc);

	// Shader module
	// Note: This generates a shader per pipeline state (ie: one for opaque, one for alpha etc), might be overkill.
	String source = sceneBindingsDesc->wgpuShaderSource +	   //
					material->descriptor()->wgpuShaderSource + //
					renderer->descriptor()->wgpuShaderSource;

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

	String label = String(material->descriptor()->label) + ":" + renderer->descriptor()->label;
	auto module = createShaderModule(gc->wgpuDevice(), source, label.c_str());

	wgpu::RenderPipelineDescriptor pipelineDescriptor;

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
	switch (rpassType) {
	case RenderPassType::shadow:
		depthStencilState.depthWriteEnabled = true; //(depthFunc != DepthFunc::undefined);
		break;
	case RenderPassType::opaque:
		depthStencilState.depthWriteEnabled = (depthFunc != DepthFunc::undefined);
		break;
	case RenderPassType::blend:
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

wgpu::RenderPipeline getOrCreateRenderPipeline(GraphicsContext* gc,		 //
											   RenderPassType rpassType, //
											   CMaterial* material,		 //
											   CBindGroup* renderer,	 //
											   DrawMode drawMode) {

//	auto cullMode = (rpassType==RenderPassType::shadow) ? CullMode::front : material->cullMode();
	auto cullMode = material->cullMode();

	return getOrCreateRenderPipeline(gc, rpassType, material->bindGroup(), material->blendMode(), material->depthFunc(),
									 cullMode, renderer, drawMode);
}

} // namespace sgd
