#include "hellotriangle.h"

#include "shadermodule.h"

namespace sgd {

namespace {
    
wgpu::RenderPipeline helloTrianglePipeline(const wgpu::Device& device, wgpu::TextureFormat colorFormat) {

	using PipelinesMap = Map<wgpu::TextureFormat, wgpu::RenderPipeline>;

	static PipelinesMap pipelinesMap;

	static Slot<wgpu::Device>* lostSlot;
	if(!lostSlot) lostSlot = wgpuDeviceLost.connect(nullptr, [=](const wgpu::Device&){ pipelinesMap.clear();});

	auto& pipeline = pipelinesMap[colorFormat];
	if (pipeline) return pipeline;

	constexpr auto shaderSource{
#include "hellotriangle.wgsl"
    };
	auto shaderModule = createShaderModule(device, shaderSource);

	wgpu::RenderPipelineDescriptor rpDesc;

	// Fragment state
	wgpu::ColorTargetState colorTarget;
	colorTarget.format = colorFormat;
	wgpu::FragmentState fragment;
	fragment.module = shaderModule;
	fragment.entryPoint = "fragmentMain";
	fragment.targetCount = 1;
	fragment.targets = &colorTarget;
	rpDesc.fragment = &fragment;

	// Vertex state
	rpDesc.vertex.module = shaderModule;
	rpDesc.vertex.entryPoint = "vertexMain";

	// Other state
	wgpu::PipelineLayoutDescriptor plDesc;
	plDesc.bindGroupLayoutCount = 0;
	rpDesc.layout = device.CreatePipelineLayout(&plDesc);

	return pipeline = device.CreateRenderPipeline(&rpDesc);
}

void drawHelloTriangle(const wgpu::Device& device, const wgpu::RenderPassEncoder& renderPass, wgpu::TextureFormat colorFormat) {

	renderPass.SetPipeline(helloTrianglePipeline(device, colorFormat));

	renderPass.Draw(3, 1, 0, 0);
}

}

void drawHelloTriangle(const wgpu::Device& device, const wgpu::Texture& renderTarget) {

	wgpu::RenderPassColorAttachment colorAttachment;
	colorAttachment.view = renderTarget.CreateView();
	colorAttachment.clearValue = {1.0f, 0.7f, 0.0f, 0.0f};
	colorAttachment.loadOp = wgpu::LoadOp::Clear;
	colorAttachment.storeOp = wgpu::StoreOp::Store;

	wgpu::RenderPassDescriptor desc;
	desc.colorAttachmentCount = 1;
	desc.colorAttachments = &colorAttachment;

	auto encoder = device.CreateCommandEncoder();

	auto renderPass = encoder.BeginRenderPass(&desc);

	drawHelloTriangle(device, renderPass, renderTarget.GetFormat());

	renderPass.End();

	auto commands = encoder.Finish();
	device.GetQueue().Submit(1, &commands);
}

} // namespace sgd
