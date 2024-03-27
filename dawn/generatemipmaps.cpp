#include "generatemipmaps.h"

#include "shadermodule.h"

namespace sgd {

void generateMipmaps(const wgpu::Device& device, const wgpu::Texture& texture) {

	auto format = texture.GetFormat();

	static wgpu::BindGroupLayout bgLayout;
	if(!bgLayout) {
		wgpu::BindGroupLayoutEntry bglEntries[2];

		// texture
		bglEntries[0].binding = 0;
		bglEntries[0].visibility = wgpu::ShaderStage::Fragment;
		bglEntries[0].texture.sampleType = wgpu::TextureSampleType::Float;
		bglEntries[0].texture.viewDimension = wgpu::TextureViewDimension::e2D;

		// sampler
		bglEntries[1].binding = 1;
		bglEntries[1].visibility = wgpu::ShaderStage::Fragment;
		bglEntries[1].sampler.type = wgpu::SamplerBindingType::Filtering;

		wgpu::BindGroupLayoutDescriptor bglDesc;
		bglDesc.entryCount = std::size(bglEntries);
		bglDesc.entries = bglEntries;

		bgLayout = device.CreateBindGroupLayout(&bglDesc);
	}

	using PipelinesMap = Map<wgpu::TextureFormat, wgpu::RenderPipeline>;
	static PipelinesMap pipelinesMap;
	auto& pipeline = pipelinesMap[format];
	if(!pipeline) {
		const char* shaderSource{
#include "generatemipmaps.wgsl"
		};

		auto shaderModule = createShaderModule(device, shaderSource);

		// Fragment state
		wgpu::RenderPipelineDescriptor rpDesc;
		wgpu::BlendState blend;
		wgpu::ColorTargetState colorTarget;
		colorTarget.format = format;
		colorTarget.blend = &blend;
		wgpu::FragmentState fragment;
		fragment.module = shaderModule;
		fragment.entryPoint = "fragmentMain";
		fragment.targetCount = 1;
		fragment.targets = &colorTarget;
		rpDesc.fragment = &fragment;

		// Vertex state
		rpDesc.vertex.module = shaderModule;
		rpDesc.vertex.entryPoint = "vertexMain";

		// Bind group layouts
		wgpu::PipelineLayoutDescriptor plDesc;
		wgpu::BindGroupLayout bgLayouts[] = {bgLayout};
		plDesc.bindGroupLayoutCount = std::size(bgLayouts);
		plDesc.bindGroupLayouts = bgLayouts;
		rpDesc.layout = device.CreatePipelineLayout(&plDesc);

		// Primitive state
		rpDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleStrip;

		pipeline = device.CreateRenderPipeline(&rpDesc);
	}

	static wgpu::Sampler sampler;
	if(!sampler) {
		wgpu::SamplerDescriptor desc;
		desc.addressModeU = desc.addressModeV = desc.addressModeW = wgpu::AddressMode::ClampToEdge;
		desc.magFilter = desc.minFilter = wgpu::FilterMode::Linear;
		desc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
		sampler = device.CreateSampler(&desc);
	}

	auto encoder = device.CreateCommandEncoder();

	for (uint32_t layer = 0; layer < texture.GetDepthOrArrayLayers(); ++layer) {

		for (uint32_t mip = 0; mip < texture.GetMipLevelCount() - 1; ++mip) {

			wgpu::TextureView srcView;
			wgpu::TextureView dstView;
			{
				wgpu::TextureViewDescriptor tvDesc;
				tvDesc.dimension = wgpu::TextureViewDimension::e2D;
				tvDesc.mipLevelCount = 1;
				tvDesc.arrayLayerCount = 1;
				tvDesc.baseMipLevel = mip;
				tvDesc.baseArrayLayer = layer;
				srcView = texture.CreateView(&tvDesc);

				tvDesc.baseMipLevel = mip + 1;
				dstView = texture.CreateView(&tvDesc);
			}

			wgpu::BindGroup bindGroup;
			{
				wgpu::BindGroupEntry bgEntries[2];

				bgEntries[0].binding = 0;
				bgEntries[0].textureView = srcView;

				bgEntries[1].binding = 1;
				bgEntries[1].sampler = sampler;

				wgpu::BindGroupDescriptor bgDesc;
				bgDesc.layout = bgLayout;
				bgDesc.entryCount = std::size(bgEntries);
				bgDesc.entries = bgEntries;
				bindGroup = device.CreateBindGroup(&bgDesc);
			}

			wgpu::RenderPassEncoder renderPass;
			{
				wgpu::RenderPassColorAttachment colorAttachment;
				colorAttachment.view = dstView;
				colorAttachment.resolveTarget = nullptr;
				colorAttachment.loadOp = wgpu::LoadOp::Clear;
				colorAttachment.storeOp = wgpu::StoreOp::Store;
				colorAttachment.clearValue = {0.0f, 1.0f, 0.0f, 0.0f};
				wgpu::RenderPassDescriptor rpDesc;
				rpDesc.colorAttachmentCount = 1;
				rpDesc.colorAttachments = &colorAttachment;
				renderPass = encoder.BeginRenderPass(&rpDesc);
			}

			renderPass.SetPipeline(pipeline);
			renderPass.SetBindGroup(0, bindGroup);
			renderPass.Draw(4);
			renderPass.End();
		}
	}

	auto commands = encoder.Finish();
	device.GetQueue().Submit(1, &commands);
}

} // namespace xros
