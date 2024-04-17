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

#if SGD_OS_MACOS
// Defined in device.mm
void* createMetalLayer(GLFWwindow* window);
#endif

namespace {

wgpu::FeatureName requiredFeatures[]{wgpu::FeatureName::TimestampQuery};
//, wgpu::FeatureName::TimestampQueryInsidePasses};

const char* enabledToggles[] = {"float32-filterable", "allow_unsafe_apis"};

const char* errorNames[] = {"No error", "Validation", "Out of memory", "Internal", "Unknown", "Device lost"};

void dawnErrorCallback(WGPUErrorType type, const char* message, void*) {
	auto name = (uint32_t)type < std::size(errorNames) ? errorNames[(uint32_t)type] : "OOPS";
	alert(String("Dawn Device error:\n") + name + "\n" + message);
	SGD_ABORT();
}

void logAdapterProps(const wgpu::Adapter& adapter) {

	wgpu::AdapterProperties props{};
	adapter.GetProperties(&props);

	log() << "### Dawn WGPU Adapter Properties:";
	log() << "### Vender name:" << (props.vendorName ? props.vendorName : "???");
	log() << "### Architecture:" << (props.architecture ? props.architecture : "???");
	log() << "### Name:" << (props.name ? props.name : "???");
	log() << "### Driver description:" << (props.driverDescription ? props.driverDescription : "???");

	Map<wgpu::AdapterType, String> adapterTypes{
		{wgpu::AdapterType::DiscreteGPU, "DiscreteGPU"},
		{wgpu::AdapterType::IntegratedGPU, "IntegratedGPU"},
		{wgpu::AdapterType::CPU, "CPU"},
		{wgpu::AdapterType::Unknown, "Unknown"},
	};
	log() << "### Adapter type:" << adapterTypes[props.adapterType];

	Map<wgpu::BackendType, String> backendTypes{
		{wgpu::BackendType::Undefined, "Undefined"}, {wgpu::BackendType::D3D12, "D3D12"},
		{wgpu::BackendType::D3D11, "D3D11"},		 {wgpu::BackendType::Vulkan, "Vulkan"},
		{wgpu::BackendType::Null, "Null"},			 {wgpu::BackendType::Metal, "Metal"},
		{wgpu::BackendType::OpenGL, "OpenGL"},		 {wgpu::BackendType::OpenGLES, "OpenGLES"},
		{wgpu::BackendType::WebGPU, "WebGPU"},
	};
	log() << "### Backend type:" << backendTypes[props.backendType];
	log() << "### Compatibility mode:" << props.compatibilityMode;
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

			logAdapterProps(adapter);

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
	CondVar<bool> ready;

	runOnMainThread([&]{
		requestWGPUDevice(adapterOptions, [&](const wgpu::Device& device) { //
			result=device;
			ready.set(true);
		});
	}, true);

	ready.waiteq(true);
	SGD_ASSERT(result);
	return result;
}

wgpu::TextureFormat preferredWGPUSwapChainFormat(const wgpu::Device& device) {

	return wgpu::TextureFormat::BGRA8Unorm;
}

wgpu::Surface createWGPUSurface(const wgpu::Device& device, GLFWwindow* window) {

	wgpu::Surface result;
	CondVar<bool> ready;

	runOnMainThread([&]{

#if SGD_OS_WINDOWS
		wgpu::SurfaceDescriptorFromWindowsHWND nativeDesc{};
		nativeDesc.hwnd = glfwGetWin32Window(window);
		nativeDesc.hinstance = GetModuleHandle(nullptr);
#elif SGD_OS_LINUX
		wgpu::SurfaceDescriptorFromXlibWindow nativeDesc{};
		nativeDesc.window = glfwGetX11Window(window);
		nativeDesc.display = glfwGetX11Display();
#elif SGD_OS_MACOS
		wgpu::SurfaceDescriptorFromMetalLayer nativeDesc{};
		nativeDesc.layer = createMetalLayer(window);
#elif SGD_OS_EMSCRIPTEN
		wgpu::SurfaceDescriptorFromCanvasHTMLSelector nativeDesc{};
		nativeDesc.selector = "#canvas";
#else
		SGD_ABORT();
#endif
		wgpu::SurfaceDescriptor surfaceDesc{};
		surfaceDesc.nextInChain = &nativeDesc;

		wgpu::Surface surface = wgpuInstanceCreateSurface(getWGPUInstance().Get(), (WGPUSurfaceDescriptor*)&surfaceDesc);
		// Can't use hits, causes weird missing symbols errors on Linux!
		//wgpu::Surface surface = getWGPUInstance().CreateSurface(&surfaceDesc);

		result=surface;
		ready.set(true);

	},true);

	ready.waiteq(true);
	SGD_ASSERT(result);
	return result;
}

wgpu::SwapChain createWGPUSwapChain(const wgpu::Device& device, const wgpu::Surface& surface, CVec2u size,
									wgpu::TextureFormat format) {
	wgpu::SwapChain result;
	CondVar<bool> ready;

	runOnMainThread([&]{
		wgpu::SwapChainDescriptor desc;
		desc.usage = wgpu::TextureUsage::RenderAttachment;
		desc.format = format;
		desc.width = size.x;
		desc.height = size.y;
#if SGD_OS_EMSCRIPTEN
		desc.presentMode = wgpu::PresentMode::Fifo;
#else
		//desc.presentMode = wgpu::PresentMode::Immediate;	// ?!?
		//desc.presentMode = wgpu::PresentMode::Mailbox;	// vsync = off
		desc.presentMode = wgpu::PresentMode::Fifo; // vsync = on
#endif
		result = device.CreateSwapChain(surface, &desc);
		ready.set(true);

	},true);

	ready.waiteq(true);
	SGD_ASSERT(result);
	return result;
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

	ready.waiteq(true);
}

#else

void  requestRender(CFunction<void()> renderFunc) {
	runOnMainThread(renderFunc, true);
}

#endif

} // namespace sgd
