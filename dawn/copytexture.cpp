#include "copytexture.h"

#include "shadermodule.h"

namespace sgd {

namespace {

const char* shaderSource{
#include "copytexture.wgsl"
};

wgpu::BindGroupLayout bindGroupLayout(const wgpu::Device& device) {

	static wgpu::BindGroupLayout bgLayout;

	if (bgLayout) return bgLayout;

	wgpu::BindGroupLayoutEntry bglEntries[2];

	// texture
	bglEntries[0].binding = 0;
	bglEntries[0].visibility = wgpu::ShaderStage::Fragment;
	bglEntries[0].texture.sampleType = wgpu::TextureSampleType::Float;
	bglEntries[0].texture.multisampled = false;
	bglEntries[0].texture.viewDimension = wgpu::TextureViewDimension::e2D;

	// sampler
	bglEntries[1].binding = 1;
	bglEntries[1].visibility = wgpu::ShaderStage::Fragment;
	bglEntries[1].sampler.type = wgpu::SamplerBindingType::Filtering;

	wgpu::BindGroupLayoutDescriptor bglDesc;
	bglDesc.entryCount = std::size(bglEntries);
	bglDesc.entries = bglEntries;

	return bgLayout = device.CreateBindGroupLayout(&bglDesc);
}

wgpu::RenderPipeline renderPipeline(const wgpu::Device& device, wgpu::TextureFormat dstFormat) {

	using PipelinesMap = Map<wgpu::TextureFormat, wgpu::RenderPipeline>;

	static PipelinesMap pipelinesMap;
	auto& pipeline = pipelinesMap[dstFormat];

	if (pipeline) return pipeline;

	auto shaderModule = createShaderModule(device, shaderSource);

	wgpu::RenderPipelineDescriptor rpDesc;

	// ***** Fragment state *****

	wgpu::ConstantEntry constEntry;
	constEntry.key = "srgbTarget";
	constEntry.value = (dstFormat == wgpu::TextureFormat::RGBA8UnormSrgb || dstFormat == wgpu::TextureFormat::BGRA8UnormSrgb);

	wgpu::ColorTargetState colorTarget;
	colorTarget.format = dstFormat;
	wgpu::BlendState blend;
	colorTarget.blend = &blend;

	wgpu::FragmentState fragment;
	fragment.module = shaderModule;
	fragment.entryPoint = "fragmentMain";
	fragment.constantCount = 1;
	fragment.constants = &constEntry;
	fragment.targetCount = 1;
	fragment.targets = &colorTarget;

	rpDesc.fragment = &fragment;

	// ***** Vertex state *****

	rpDesc.vertex.module = shaderModule;
	rpDesc.vertex.entryPoint = "vertexMain";

	// ***** Pipeline layout (BindingGroupLayouts) *****

	auto bgLayout = bindGroupLayout(device);
	wgpu::PipelineLayoutDescriptor plDesc{};
	plDesc.bindGroupLayoutCount = 1;
	plDesc.bindGroupLayouts = &bgLayout;
	rpDesc.layout = device.CreatePipelineLayout(&plDesc);

	// ***** Primitive state *****

	rpDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleStrip;

	return pipeline = device.CreateRenderPipeline(&rpDesc);
}

wgpu::BindGroup bindGroup(const wgpu::Device& device, const wgpu::TextureView& srcTexture) {

	static wgpu::Sampler sampler;
	if (!sampler) {
		wgpu::SamplerDescriptor desc{};
		desc.minFilter = wgpu::FilterMode::Linear;
		sampler = device.CreateSampler(&desc);
	}

	wgpu::BindGroupEntry bgEntries[2];

	bgEntries[0].binding = 0;
	bgEntries[0].textureView = srcTexture;

	bgEntries[1].binding = 1;
	bgEntries[1].sampler = sampler;

	wgpu::BindGroupDescriptor bgDesc;
	bgDesc.layout = bindGroupLayout(device);
	bgDesc.entryCount = std::size(bgEntries);
	bgDesc.entries = bgEntries;
	auto bindGroup = device.CreateBindGroup(&bgDesc);

	return bindGroup;
}

} // namespace

void drawTexture(const wgpu::Device& device, const wgpu::Texture& srcTexture, CRectu srcRect, const wgpu::Texture& dstTexture,
				 CRectu dstRect) {

	// TODO, srcRect
	SGD_ASSERT(Rectu(0,0,srcTexture.GetWidth(),srcTexture.GetHeight()) == srcRect);

	auto encoder = device.CreateCommandEncoder();

	wgpu::RenderPassEncoder renderPass;
	{
		wgpu::RenderPassColorAttachment colorAttachment;
		colorAttachment.view = dstTexture.CreateView();
		colorAttachment.resolveTarget = nullptr;
		colorAttachment.loadOp = wgpu::LoadOp::Load;
		colorAttachment.storeOp = wgpu::StoreOp::Store;
		colorAttachment.clearValue = {0.0f, 0.0f, 0.0f, 0.0f};
		wgpu::RenderPassDescriptor renderpassInfo;
		renderpassInfo.colorAttachmentCount = 1;
		renderpassInfo.colorAttachments = &colorAttachment;

		renderPass = encoder.BeginRenderPass(&renderpassInfo);
	}

	renderPass.SetViewport(dstRect.min.x, dstRect.min.y, dstRect.size().x, dstRect.size().y, 0, 1);
	renderPass.SetPipeline(renderPipeline(device, dstTexture.GetFormat()));
	renderPass.SetBindGroup(0, bindGroup(device, srcTexture.CreateView()));
	renderPass.Draw(4);
	renderPass.End();

	auto commands = encoder.Finish();
	device.GetQueue().Submit(1, &commands);
}

void copyTexture(const wgpu::Device& device, const wgpu::Texture& srcTexture, CVec2u srcOrigin, const wgpu::Texture& dstTexture,
				 CVec2u dstOrigin, CVec2u size) {

	if (srcTexture.GetFormat() != dstTexture.GetFormat()) {
		Rectu srcRect{srcOrigin, srcOrigin + size};
		Rectu dstRect{dstOrigin, dstOrigin + size};
		drawTexture(device, srcTexture, srcRect, dstTexture, dstRect);
		return;
	}

	wgpu::ImageCopyTexture src{};
	src.texture = srcTexture;
	src.origin = {srcOrigin.x, srcOrigin.y, 0};

	wgpu::ImageCopyTexture dst{};
	dst.texture = dstTexture;
	dst.origin = {dstOrigin.x, dstOrigin.y, 0};

	wgpu::Extent3D extent{};
	extent.width = size.x;
	extent.height = size.y;

	auto encoder = device.CreateCommandEncoder();
	encoder.CopyTextureToTexture(&src, &dst, &extent);

	auto commands = encoder.Finish();
	device.GetQueue().Submit(1, &commands);
}

void copyTexture(const wgpu::Device& device, const wgpu::Texture& srcTexture, const wgpu::Texture& dstTexture) {

	if (srcTexture.GetWidth() != dstTexture.GetWidth() || srcTexture.GetHeight() != dstTexture.GetHeight()) {
		Rectu srcRect(0,0,srcTexture.GetWidth(), srcTexture.GetHeight());
		Rectu dstRect(0,0,dstTexture.GetWidth(), dstTexture.GetHeight());
		drawTexture(device, srcTexture, srcRect, dstTexture, dstRect);
		return;
	}
	copyTexture(device, srcTexture, {0, 0}, dstTexture, {0, 0}, {srcTexture.GetWidth(), srcTexture.GetHeight()});
}

} // namespace sgd
