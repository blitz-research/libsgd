#include <cassert>
#include <cstdlib>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include "GLFW/glfw3native.h"

#import <QuartzCore/CAMetalLayer.h>

#include <dawn/dawn_proc.h>
#include <dawn/native/DawnNative.h>
#include <webgpu/webgpu_cpp.h>

// ***** OPTIONS *****

uint32_t width = 640;
uint32_t height = 480;

// Set to false to disable bindgroup - program works normally
// Set to true to enable bindgroup - program displays a magenta view. In bigger apps this may crash too.
bool bindGroupEnabled = true;

// *****

GLFWwindow* glfwWindow;

wgpu::Instance wgpuInstance;

wgpu::Adapter wgpuAdapter;
wgpu::Device wgpuDevice;

wgpu::TextureFormat wgpuSurfaceFormat;
wgpu::Surface wgpuSurface;

wgpu::BindGroupLayout wgpuBindGroupLayout;
wgpu::BindGroup wgpuBindGroup;

wgpu::RenderPipeline wgpuPipeline;

auto shaderSource = R"(

@group(0) @binding(0) var texture: texture_2d_array<f32>;

@vertex fn vertexMain(@builtin(vertex_index) vertexId: u32) -> @builtin(position) vec4f {
    const verts = array<vec2f, 3>(vec2f( 0.0,  0.5), vec2f(-0.5, -0.5), vec2f( 0.5, -0.5));

    return vec4f(verts[vertexId], 0.0, 1.0);
}

@fragment fn fragmentMain() -> @location(0) vec4f {

    return vec4f(1.0, 0.0, 0.0, 1.0);
}

)";

void createWindow() {

	if (!glfwInit()) std::abort();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindow = glfwCreateWindow((int)width, (int)height, "Metal Test", nullptr, nullptr);
	assert(glfwWindow);
}

void createInstance() {
	dawnProcSetProcs(&dawn::native::GetProcs());
	wgpu::InstanceDescriptor desc{};
	desc.features.timedWaitAnyEnable = true;
	wgpuInstance = wgpu::CreateInstance(&desc);
	assert(wgpuInstance);
}

void deviceErrorCallback(WGPUErrorType type, const char* message,void*) {
	printf("### Dawn error: %s\n",message);
	fflush(stdout);
	std::exit(1);
};

void createDevice()	{
	wgpu::RequestAdapterOptions adapterOpts{};

	(void)wgpuInstance.RequestAdapter( //
		&adapterOpts, wgpu::CallbackMode::AllowProcessEvents,
		[](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, const char* message) {
			//
			assert(status == wgpu::RequestAdapterStatus::Success);
			wgpuAdapter = std::move(adapter);

			wgpu::DeviceDescriptor devDesc{};
            devDesc.uncapturedErrorCallbackInfo.callback = &deviceErrorCallback;

			(void)wgpuAdapter.RequestDevice( //
				&devDesc, wgpu::CallbackMode::AllowProcessEvents,
				[](wgpu::RequestDeviceStatus status, wgpu::Device device, const char* message) {
					//
					assert(status == wgpu::RequestDeviceStatus::Success);


					wgpuDevice = std::move(device);
				});
			wgpuInstance.ProcessEvents();
		});
	wgpuInstance.ProcessEvents();
	assert(wgpuDevice);
}

void createSurface(){
	NSWindow* nsWindow = glfwGetCocoaWindow(glfwWindow);
	NSView* view = [nsWindow contentView];

    auto layer = [CAMetalLayer layer];
    
//    layer.displaySyncEnabled = true;
	[view setWantsLayer:YES];
    [view setLayer:layer];

	// Use retina if the window was created with retina support.
	[[view layer] setContentsScale:[nsWindow backingScaleFactor]];

	wgpu::SurfaceDescriptorFromMetalLayer nativeDesc{};
	nativeDesc.layer = [view layer];

	wgpu::SurfaceDescriptor surfaceDesc{};
	surfaceDesc.nextInChain = &nativeDesc;

	wgpuSurface = wgpuInstance.CreateSurface(&surfaceDesc);
	assert(wgpuSurface);

	wgpu::SurfaceCapabilities surfCaps{};
	if (!wgpuSurface.GetCapabilities(wgpuDevice.GetAdapter(), &surfCaps) || !surfCaps.formatCount) {
		abort();
	}
	wgpuSurfaceFormat = surfCaps.formats[0];

	wgpu::SurfaceConfiguration config{};
	config.device = wgpuDevice;
	config.width = width;
	config.height = height;
	config.format = wgpuSurfaceFormat;
	config.usage = wgpu::TextureUsage::RenderAttachment;
	config.presentMode = wgpu::PresentMode::Fifo;
	config.alphaMode = wgpu::CompositeAlphaMode::Opaque;
	wgpuSurface.Configure(&config);
}

void createBindGroup() {	// Create BindGroupLayout and BindGroup

	// Create layout
	wgpu::BindGroupLayoutEntry bglEntries[1]{};
	bglEntries[0].binding = 0;
	bglEntries[0].visibility = wgpu::ShaderStage::Fragment;
	bglEntries[0].texture.viewDimension = wgpu::TextureViewDimension::e2DArray;
	bglEntries[0].texture.sampleType=wgpu::TextureSampleType::Float;
	wgpu::BindGroupLayoutDescriptor bglDesc{};
	bglDesc.entries=bglEntries;
	bglDesc.entryCount=std::size(bglEntries);
	wgpuBindGroupLayout = wgpuDevice.CreateBindGroupLayout(&bglDesc);

	// Create texture
	wgpu::TextureDescriptor texDesc{};
	texDesc.size = {1024,1024,16};
	texDesc.format = wgpu::TextureFormat::RGBA8Unorm;
	texDesc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
	auto wgpuTexture = wgpuDevice.CreateTexture(&texDesc);

	wgpu::TextureViewDescriptor tvDesc{};
	tvDesc.dimension = wgpu::TextureViewDimension::e2DArray;

	// Create bind group
	wgpu::BindGroupEntry bgEntries[1];
	bgEntries[0].binding = 0;
	bgEntries[0].textureView = wgpuTexture.CreateView(&tvDesc);

	wgpu::BindGroupDescriptor bgDesc{};
	bgDesc.layout = wgpuBindGroupLayout;
	bgDesc.entries = bgEntries;
	bgDesc.entryCount = std::size(bgEntries);
	wgpuBindGroup = wgpuDevice.CreateBindGroup(&bgDesc);
}

// Create pipeline
void createPipeline()	{
	// Compile shader
	wgpu::ShaderModuleWGSLDescriptor wgslDesc{};
	wgslDesc.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
	wgslDesc.code = shaderSource;
	wgpu::ShaderModuleDescriptor desc{};
	desc.nextInChain = (wgpu::ChainedStruct*)&wgslDesc;
	auto shaderModule = wgpuDevice.CreateShaderModule(&desc);
	assert(shaderModule);

	wgpu::RenderPipelineDescriptor rpDesc{};

	// Fragment state
	wgpu::ColorTargetState colorTarget{};
	colorTarget.format = wgpuSurfaceFormat;
	wgpu::FragmentState fragment{};
	fragment.module = shaderModule;
	fragment.entryPoint = "fragmentMain";
	fragment.targetCount = 1;
	fragment.targets = &colorTarget;
	rpDesc.fragment = &fragment;

	// Vertex state
	rpDesc.vertex.module = shaderModule;
	rpDesc.vertex.entryPoint = "vertexMain";

	// Bind group layouts
	wgpu::PipelineLayoutDescriptor plDesc{};
	plDesc.bindGroupLayouts = &wgpuBindGroupLayout;
	plDesc.bindGroupLayoutCount = (int)bindGroupEnabled;
	rpDesc.layout = wgpuDevice.CreatePipelineLayout(&plDesc);

	wgpuPipeline = wgpuDevice.CreateRenderPipeline(&rpDesc);
}

int main() {

    createWindow();

    createInstance();

    createDevice();

    createSurface();

    if(bindGroupEnabled) createBindGroup();

    createPipeline();

	// Render loop!
//	for (;;) {
	for(int i=0;i<30;++i) {
        
        puts("### Tick");

        // Note: without this, there's no vsync on MacOS
		glfwPollEvents();
//		if (glfwWindowShouldClose(glfwWindow)) break;

		// Begin rendering
		auto encoder = wgpuDevice.CreateCommandEncoder();

		// Render directly to swapchain
		struct wgpu::SurfaceTexture surfaceTexture {};
		wgpuSurface.GetCurrentTexture(&surfaceTexture);
		assert(surfaceTexture.status == wgpu::SurfaceGetCurrentTextureStatus::Success);
		auto renderTarget = surfaceTexture.texture;

		// Create renderpass
		wgpu::RenderPassColorAttachment colorAttachment{};
		colorAttachment.view = renderTarget.CreateView();
		colorAttachment.clearValue = {1.0f, 0.7f, 0.0f, 0.0f};
		colorAttachment.loadOp = wgpu::LoadOp::Clear;
		colorAttachment.storeOp = wgpu::StoreOp::Store;
		wgpu::RenderPassDescriptor desc{};
		desc.colorAttachmentCount = 1;
		desc.colorAttachments = &colorAttachment;
		auto renderPass = encoder.BeginRenderPass(&desc);

		if(bindGroupEnabled) renderPass.SetBindGroup(0, wgpuBindGroup);

		// Render triangle
		renderPass.SetPipeline(wgpuPipeline);
		renderPass.Draw(3, 1, 0, 0);
		renderPass.End();

		auto commands = encoder.Finish();
		wgpuDevice.GetQueue().Submit(1, &commands);

		// SwapBuffers
		wgpuSurface.Present();

		wgpuDevice.GetAdapter().GetInstance().ProcessEvents();
	}

	wgpuDevice.Destroy();

	glfwDestroyWindow(glfwWindow);
	glfwTerminate();
	return 0;
}
