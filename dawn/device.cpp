#include "device.h"

#include <GLFW/glfw3.h>

#if SGD_OS_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32 1
#include <GLFW/glfw3native.h>
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
					SGD_ASSERT(status == WGPURequestDeviceStatus_Success);
					auto device = wgpu::Device::Acquire(cDevice);
					device.SetUncapturedErrorCallback(&dawnErrorCallback, nullptr);
					auto callback = (Function<void(const wgpu::Device&)>*)userdata;
					(*callback)(device);
					delete callback;
					//
				},
				userdata);
		},
		userdata);

	getWGPUInstance().ProcessEvents();
}

wgpu::Device createWGPUDevice(const wgpu::RequestAdapterOptions& adapterOptions) {

	wgpu::Device result;

	requestWGPUDevice(adapterOptions, [&](const wgpu::Device& device) { //
		result = device;
	});

	return result;
}

wgpu::TextureFormat preferredWGPUSwapChainFormat(const wgpu::Device& device) {
	return wgpu::TextureFormat::BGRA8Unorm;
}

wgpu::Surface createWGPUSurface(const wgpu::Device& device, GLFWwindow* window) {

	wgpu::SurfaceDescriptor surfaceDesc{};
#if SGD_OS_WINDOWS
	wgpu::SurfaceDescriptorFromWindowsHWND nativeDesc{};
	nativeDesc.hwnd = glfwGetWin32Window(window);
	nativeDesc.hinstance = GetModuleHandle(nullptr);
#elif SGD_OS_LINUX
	wgpu::SurfaceDescriptorFromXlibWindow nativeDesc{};
	nativeDesc.window = glfwGetX11Window(window);
	nativeDesc.display = glfwGetX11Display();
#elif SGD_OS_EMSCRIPTEN
	wgpu::SurfaceDescriptorFromCanvasHTMLSelector nativeDesc{};
	nativeDesc.selector = "#canvas";
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

EM_CALLBACK void sgd_requestAnimationFrameOK(Function<void()>* func) {
	(*func)();
	delete func;
}

// clang-format off
EM_JS(void, sgd_requestAnimationFrame, (Function<void()>* func), {
	requestAnimationFrame(() => {
		_sgd_requestAnimationFrameOK(func);
	});
});

// clang-format on

void requestRender(CFunction<void()> renderFunc) {

	CondVar<bool> ready;

	auto func = new Function<void()>([&ready, renderFunc] { //
		ready.set(true);
		renderFunc();
	});

	runOnMainThread([func] { //
		sgd_requestAnimationFrame(func);
	}, true);

	ready.wait(true);
}

#else

void requestRender(CFunction<void()> renderFunc) {
	runOnMainThread(renderFunc, true);
}

#endif

} // namespace sgd
