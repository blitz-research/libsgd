#include "device.h"

#if SGD_OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef min
#undef max
#elif SGD_OS_LINUX
#define GLFW_EXPOSE_NATIVE_X11 1
#define GLFW_EXPOSE_NATIVE_GLX 1
#include <GLFW/glfw3native.h>
#undef Success
#undef Always
#undef None
#endif

namespace sgd {

namespace {

wgpu::FeatureName requiredFeatures[]{wgpu::FeatureName::TimestampQuery};
//, wgpu::FeatureName::TimestampQueryInsidePasses};

const char* enabledToggles[] = {"float32-filterable", "allow_unsafe_apis"};

const char* errorNames[] = {"No error", "Validation", "Out of memory", "Internal", "Unknown", "Device lost"};

void dawnErrorCallback(WGPUErrorType type, const char* message, void*) {
	auto name = (uint32_t)type < std::size(errorNames) ? errorNames[(uint32_t)type] : "OOPS";
	log() << "!!! Dawn error:" << name << message;
	SGD_ABORT();
}

wgpu::Device requestedDevice;

} // namespace

const wgpu::Instance& getWGPUInstance() {
	static wgpu::Instance instance = wgpu::CreateInstance();
	return instance;
}

void requestWGPUDevice(const wgpu::RequestAdapterOptions& adapterOptions,
					   CFunction<void(const wgpu::Device& device)> callbackFunc) {

	void* userdata = new Function<void(const wgpu::Device& device)>(callbackFunc);

	getWGPUInstance().RequestAdapter(
		&adapterOptions,
		[](WGPURequestAdapterStatus status, WGPUAdapter cAdapter, char const* message, void* userdata) {
			//
			SGD_ASSERT(status == WGPURequestAdapterStatus_Success);
			auto adapter = wgpu::Adapter::Acquire(cAdapter);
			//
			adapter.RequestDevice(
				nullptr,
				[](WGPURequestDeviceStatus status, WGPUDevice cDevice, const char* message, void* userdata) {
					//
					requestedDevice = wgpu::Device::Acquire(cDevice);
					requestedDevice.SetUncapturedErrorCallback(&dawnErrorCallback, nullptr);
					auto callback = (Function<void(const wgpu::Device& device)>*)userdata;
					(*callback)(requestedDevice);
					delete callback;
					//
				},
				userdata);
		},
		userdata);
}

wgpu::Device createWGPUDevice(const wgpu::RequestAdapterOptions& adapterOptions) {

	if(requestedDevice) return requestedDevice;

	requestWGPUDevice(adapterOptions, [&](const wgpu::Device& result){
	});

	getWGPUInstance().ProcessEvents();

	SGD_ASSERT(requestedDevice);
	return requestedDevice;
}

wgpu::TextureFormat preferredWGPUSwapChainFormat(const wgpu::Device& device) {
	return wgpu::TextureFormat::BGRA8Unorm;
}

wgpu::Surface createWGPUSurface(const wgpu::Device& device, void* nativeWindow) {

	wgpu::SurfaceDescriptor surfaceDesc{};
#if SGD_OS_WINDOWS
	wgpu::SurfaceDescriptorFromWindowsHWND nativeDesc{};
	nativeDesc.hwnd = nativeWindow;
	nativeDesc.hinstance = GetModuleHandle(nullptr);
#elif SGD_OS_LINUX
	wgpu::SurfaceDescriptorFromXlibWindow nativeDesc{};
	nativeDesc.window = glfwGetX11Window(window);
	nativeDesc.display = glfwGetX11Display();
#elif SGD_OS_EMSCRIPTEN
	wgpu::SurfaceDescriptorFromCanvasHTMLSelector nativeDesc{};
	nativeDesc.selector = (const char*)nativeWindow;
#else
	SGD_ABORT();
#endif
	surfaceDesc.nextInChain = &nativeDesc;

	wgpu::Surface surface = wgpuInstanceCreateSurface(getWGPUInstance().Get(), (WGPUSurfaceDescriptor*)&surfaceDesc);
	//	wgpu::Surface surface = getWGPUInstance().CreateSurface(&surfaceDesc);

	return surface;
}

wgpu::SwapChain createWGPUSwapChain(const wgpu::Device& device, const wgpu::Surface& surface, CVec2u size,
									wgpu::TextureFormat format) {
	wgpu::SwapChainDescriptor desc;
	desc.usage = wgpu::TextureUsage::RenderAttachment;
	desc.format = format;
	desc.width = size.x;
	desc.height = size.y;
#if SGD_OS_EMSCRIPTEN
	desc.presentMode = wgpu::PresentMode::Fifo;
#else
	desc.presentMode = wgpu::PresentMode::Mailbox;
#endif

	auto swapChain = device.CreateSwapChain(surface, &desc);
	SGD_ASSERT(swapChain);

	return swapChain;
}

#if SGD_OS_EMSCRIPTEN

EM_CALLBACK void sgd_requestRenderSuccess(Function<void()>* func_ptr) {
	(*func_ptr)();
	delete func_ptr;
}

// clang-format off
EM_JS(void, sgd_requestRender, (Function<void()>* func_ptr), {
	requestAnimationFrame(() => {
		_sgd_requestRenderSuccess(func_ptr);
	});
});
// clang-format on

void requestRender(CFunction<void()> renderFunc) {
	sgd_requestRender(new Function<void()>(renderFunc));
}

#else

void requestRender(CFunction<void()> renderFunc) {

	if(!isMainThread()) {
		runOnMainThread(renderFunc, true);
		return;
	}

	static Deque<Function<void()>> renderQueue;

	renderQueue.push_back(renderFunc);

	if(renderQueue.size()>1) return;

	while(!renderQueue.empty()) {
		auto func = renderQueue.front();
		func();
		renderQueue.pop_front();
	}
}

#endif

} // namespace sgd
