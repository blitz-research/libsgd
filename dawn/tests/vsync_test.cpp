#include <cassert>
#include <cstdlib>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <GLFW/glfw3native.h>

#include <dawn/dawn_proc.h>
#include <dawn/native/DawnNative.h>
#include <webgpu/webgpu_cpp.h>

// ***** OPTIONS *****

wgpu::BackendType wgpuBackendType = wgpu::BackendType::Vulkan;	// Severe user input lag
//wgpu::BackendType wgpuBackendType = wgpu::BackendType::D3D12;	// No problemo

bool vsyncEnabled = false;
//bool vsyncEnabled = true;	// Use this to fix Vulkan input lag, would really rather not have to use this.

wgpu::PresentMode wgpuPresentMode = wgpu::PresentMode::Fifo;			// only mnode guaranteed to be supported on Vulkan?
//wgpu::PresentMode wgpuPresentMode = wgpu::PresentMode::FifoRelaxed;	// Suffers input lag
//wgpu::PresentMode wgpuPresentMode = wgpu::PresentMode::Mailbox;		// skips frames and thrashes cpu/gpu.
//wgpu::PresentMode wgpuPresentMode = wgpu::PresentMode::Immediate;		// no input lag but no vsync + tearing.

// *****

GLFWwindow* glfwWindow;

wgpu::Instance wgpuInstance;

wgpu::Adapter wgpuAdapter;
wgpu::Device wgpuDevice;

wgpu::TextureFormat wgpuSurfaceFormat;
wgpu::Surface wgpuSurface;

wgpu::RenderPipeline wgpuPipeline;

wgpu::Color wgpuClearColor;

wgpu::Future wgpuWorkDone;

auto shaderSource = R"(

@vertex fn vertexMain(@builtin(vertex_index) vertexId: u32) -> @builtin(position) vec4f {
    const verts = array<vec2f, 3>(vec2f( 0.0,  0.5), vec2f(-0.5, -0.5), vec2f( 0.5, -0.5));

    return vec4f(verts[vertexId], 0.0, 1.0);
}

@fragment fn fragmentMain() -> @location(0) vec4f {

    return vec4f(1.0, 0.0, 0.0, 1.0);
}

)";

int main() {

	uint32_t width = 640;
	uint32_t height = 480;

	// Create window
	{
		if (!glfwInit()) std::abort();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindow = glfwCreateWindow((int)width, (int)height, "VSync test", nullptr, nullptr);
		assert(glfwWindow);

		glfwSetKeyCallback(glfwWindow, [](GLFWwindow*, int key, int scancode, int action, int mods) {
			if(action!=GLFW_PRESS) return;
			printf("KeyHit key=%i\n", key);
			fflush(stdout);
			wgpuClearColor.r = 1 - wgpuClearColor.r;
		});
	}

	// Create instance
	{
		dawnProcSetProcs(&dawn::native::GetProcs());
		wgpu::InstanceDescriptor desc{};
		desc.features.timedWaitAnyEnable = true;
		wgpuInstance = wgpu::CreateInstance(&desc);
		assert(wgpuInstance);
	}

	// Create adapter and device
	{
		wgpu::RequestAdapterOptions adapterOpts{};
		adapterOpts.backendType = wgpuBackendType;

		wgpuInstance.RequestAdapter( //
			&adapterOpts, wgpu::CallbackMode::AllowProcessEvents,
			[](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, const char* message) {
				//
				assert(status == wgpu::RequestAdapterStatus::Success);
				wgpuAdapter = std::move(adapter);

				wgpu::DeviceDescriptor devDesc{};

				wgpuAdapter.RequestDevice( //
					&devDesc, wgpu::CallbackMode::AllowProcessEvents,
					[](wgpu::RequestDeviceStatus status, wgpu::Device device, const char* message) {
						//
						assert(status == wgpu::RequestDeviceStatus::Success);
						wgpuDevice = std::move(device);
					});
			});
		wgpuInstance.ProcessEvents();
		wgpuInstance.ProcessEvents();
		assert(wgpuDevice);
	}

	// Create and configure surface
	{
		wgpu::SurfaceDescriptorFromWindowsHWND nativeDesc{};
		nativeDesc.hwnd = glfwGetWin32Window(glfwWindow);
		nativeDesc.hinstance = GetModuleHandle(nullptr);
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
		config.presentMode = wgpuPresentMode;
		config.viewFormatCount = 0;
		config.viewFormats = nullptr;
		config.alphaMode = wgpu::CompositeAlphaMode::Opaque;
		wgpuSurface.Configure(&config);
	}

	// Create pipeline
	{
		// Compile shader
		wgpu::ShaderModuleWGSLDescriptor wgslDesc{};
		wgslDesc.sType = wgpu::SType::ShaderSourceWGSL;
		wgslDesc.code = shaderSource;
		wgpu::ShaderModuleDescriptor desc{};
		desc.nextInChain = (wgpu::ChainedStruct*)&wgslDesc;
		auto shaderModule = wgpuDevice.CreateShaderModule(&desc);
		assert(shaderModule);

		wgpu::RenderPipelineDescriptor rpDesc{};

		// Fragment state
		wgpu::ColorTargetState colorTarget;
		colorTarget.format = wgpuSurfaceFormat;
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
		rpDesc.layout = wgpuDevice.CreatePipelineLayout(&plDesc);

		wgpuPipeline = wgpuDevice.CreateRenderPipeline(&rpDesc);
	}

	// Render loop!
	for (;;) {

		glfwPollEvents();
		if (glfwWindowShouldClose(glfwWindow)) break;

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
		colorAttachment.clearValue = wgpuClearColor;//{1.0f, 0.7f, 0.0f, 0.0f};
		colorAttachment.loadOp = wgpu::LoadOp::Clear;
		colorAttachment.storeOp = wgpu::StoreOp::Store;
		wgpu::RenderPassDescriptor desc{};
		desc.colorAttachmentCount = 1;
		desc.colorAttachments = &colorAttachment;
		auto renderPass = encoder.BeginRenderPass(&desc);

		// Render triangle
		renderPass.SetPipeline(wgpuPipeline);
		renderPass.Draw(3, 1, 0, 0);
		renderPass.End();

		// Is this in the right place?
		if (vsyncEnabled) {
			// Wait for previous work to finished before submitting any new work.
			if(wgpuWorkDone.id) {
				// wait 5 seconds
				wgpuInstance.WaitAny(wgpuWorkDone, 5000000000);
				wgpuWorkDone.id = 0;
			}
			wgpuWorkDone = wgpuDevice.GetQueue().OnSubmittedWorkDone(
				wgpu::CallbackMode::WaitAnyOnly, [](wgpu::QueueWorkDoneStatus status) {});
		}

		// End rendering (begin on GPU!)
		auto commands = encoder.Finish();
		wgpuDevice.GetQueue().Submit(1, &commands);

		// SwapBuffers
		wgpuSurface.Present();

		// Is this the best place for this?
		wgpuDevice.GetAdapter().GetInstance().ProcessEvents();
	}

	// Does this destroy *everything*?
	wgpuDevice.Destroy();

	glfwDestroyWindow(glfwWindow);
	glfwTerminate();
	return 0;
}
