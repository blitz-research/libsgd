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
//, wgpu::FeatureName::Float32Filterable};
//, wgpu::FeatureName::TimestampQueryInsidePasses};

const char* enabledToggles[] = {"allow_unsafe_apis"};
//, "float32-filterable"

void dawnDeviceLostCallback(WGPUDevice const * device, WGPUDeviceLostReason reason, char const * message, void * userdata) {
	static const char* names[] = {"Undefined", "Unknown", "Destroyed","InstanceDropped","FailedCreation"};

	if(reason==WGPUDeviceLostReason_InstanceDropped) return;

	auto rname = (uint32_t)reason < std::size(names) ? names[(uint32_t)reason] : "OOPS";

//	SGD_LOG << "Dawn device lost:" << rname;
//	SGD_LOG << message;
	SGD_PANIC(String("Dawn device lost: ") + rname + "\n"+ message);
}

void dawnErrorCallback(WGPUErrorType type, const char* message, void*) {
	static const char* names[] = {"NoError", "Validation", "Out of memory", "Internal", "Unknown", "Device lost"};

	auto tname = (uint32_t)type < std::size(names) ? names[(uint32_t)type] : "OOPS";

	SGD_LOG << "Dawn device error message:"<<tname;
	SGD_LOG << message;
	alert(String("Dawn Device error:\n") + tname + "\n" + message);
	SGD_ABORT();
}

void dawnLoggingCallback(WGPULoggingType type, const char* message, void*) {
	static const char* names[]= {"???", "Verbose", "Info", "Warning", "Error"};

	auto tname = (uint32_t)type < std::size(names) ? names[(uint32_t)type] : "OOPS";

	switch(type) {
	case WGPULoggingType_Verbose:
#if SGD_CONFIG_DEBUG
		SGD_LOG << "Dawn:" << message;
#endif
		break;
	case WGPULoggingType_Info:
		SGD_LOG << "Dawn info:" << message;
		break;
	case WGPULoggingType_Warning:
		SGD_LOG << "Dawn warning:" << message;
#if SGD_CONFIG_DEBUG
		alert(String("Dawn warning: ") + message);
#endif
		break;
	case WGPULoggingType_Error:
		SGD_LOG << "Dawn error:" << message;
		alert(String("Dawn error: ") + message);
		SGD_ABORT();
		break;
	default:
		SGD_PANIC("Dawn logging error");
	}
}

void logAdapterProps(const wgpu::Adapter& adapter) {

	wgpu::AdapterProperties props{};

	// Crashes on Linux sometimes!
	// adapter.GetProperties(&props);
	wgpuAdapterGetProperties(adapter.Get(), reinterpret_cast<WGPUAdapterProperties*>(&props));

	SGD_LOG << "Dawn WGPU Adapter Properties:";
	SGD_LOG << "Vender name:" << (props.vendorName ? props.vendorName : "???");
	SGD_LOG << "Architecture:" << (props.architecture ? props.architecture : "???");
	SGD_LOG << "Name:" << (props.name ? props.name : "???");
	SGD_LOG << "Driver description:" << (props.driverDescription ? props.driverDescription : "???");

	Map<wgpu::AdapterType, String> adapterTypes{
		{wgpu::AdapterType::DiscreteGPU, "DiscreteGPU"},
		{wgpu::AdapterType::IntegratedGPU, "IntegratedGPU"},
		{wgpu::AdapterType::CPU, "CPU"},
		{wgpu::AdapterType::Unknown, "Unknown"},
	};
	SGD_LOG << "Adapter type:" << adapterTypes[props.adapterType];

	Map<wgpu::BackendType, String> backendTypes{
		{wgpu::BackendType::Undefined, "Undefined"}, {wgpu::BackendType::D3D12, "D3D12"},
		{wgpu::BackendType::D3D11, "D3D11"},		 {wgpu::BackendType::Vulkan, "Vulkan"},
		{wgpu::BackendType::Null, "Null"},			 {wgpu::BackendType::Metal, "Metal"},
		{wgpu::BackendType::OpenGL, "OpenGL"},		 {wgpu::BackendType::OpenGLES, "OpenGLES"},
		{wgpu::BackendType::WebGPU, "WebGPU"},
	};
	SGD_LOG << "Backend type:" << backendTypes[props.backendType];
	SGD_LOG << "Compatibility mode:" << props.compatibilityMode;
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
			if (status != WGPURequestAdapterStatus_Success) {
				SGD_PANIC("Dawn RequestAdapter() failed");
			}
			auto adapter = wgpu::Adapter::Acquire(cAdapter);

			logAdapterProps(adapter);

			wgpu::DeviceDescriptor devDesc{};
			devDesc.deviceLostCallbackInfo.callback = &dawnDeviceLostCallback;
			devDesc.uncapturedErrorCallbackInfo.callback = &dawnErrorCallback;

			wgpu::DawnTogglesDescriptor togglesDesc{};
			auto feature = wgpu::FeatureName::TimestampQuery;
			if (adapter.HasFeature(feature)) {
				devDesc.nextInChain = &togglesDesc;
				devDesc.requiredFeatureCount = 1;
				devDesc.requiredFeatures = &feature;
				togglesDesc.enabledToggleCount = std::size(enabledToggles);
				togglesDesc.enabledToggles = enabledToggles;
			}

			adapter.RequestDevice(
				&devDesc,
				[](WGPURequestDeviceStatus status, WGPUDevice cDevice, const char* message, void* userdata) {
					//
					if (status != WGPURequestDeviceStatus_Success) {
						SGD_PANIC("Dawn RequestDevice() failed");
					}
					auto device = wgpu::Device::Acquire(cDevice);

					device.SetLoggingCallback(&dawnLoggingCallback, nullptr);

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

	runOnMainThread(
		[&] {
			requestWGPUDevice(adapterOptions, [&](const wgpu::Device& device) { //
				result = device;
				ready = true;
			});
		},
		true);

	ready.waiteq(true);
	SGD_ASSERT(result);
	return result;
}

wgpu::Surface createWGPUSurface(const wgpu::Device& device, GLFWwindow* window) {

	wgpu::Surface result;
	CondVar<bool> ready;

	runOnMainThread(
		[&] {

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
#endif
			wgpu::SurfaceDescriptor surfaceDesc{};
			surfaceDesc.nextInChain = &nativeDesc;

			wgpu::Surface surface = wgpuInstanceCreateSurface(getWGPUInstance().Get(), (WGPUSurfaceDescriptor*)&surfaceDesc);
			// Can't use hits, causes weird missing symbols errors on Linux!
			// wgpu::Surface surface = getWGPUInstance().CreateSurface(&surfaceDesc);

			result = surface;
			ready = true;
		},
		true);

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

	runOnMainThread(
		[func] { //
			sgd_requestAnimationFrame(func);
		},
		true);

	ready.waiteq(true);
}

#else

void requestRender(CFunction<void()> renderFunc) {
	runOnMainThread(renderFunc, true);
}

#endif

} // namespace sgd
