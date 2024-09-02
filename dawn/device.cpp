#include "device.h"

#include "dawn/dawn_proc.h"
#include "dawn/native/DawnNative.h"

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

bool g_validationEnabled = true;
auto init0 = configVarChanged("dawn.validationEnabled").connect(nullptr,[](CString value){
	g_validationEnabled = truthiness(value);
});

bool g_robustnessEnabled = true;
auto init1 = configVarChanged("dawn.robustnessEnabled").connect(nullptr,[](CString value){
	g_validationEnabled = truthiness(value);
});

//wgpu::FeatureName requiredFeatures[]{wgpu::FeatureName::TimestampQuery};
//, wgpu::FeatureName::Float32Filterable};
//, wgpu::FeatureName::TimestampQueryInsidePasses};

//const char* enabledToggles[] = {"allow_unsafe_apis"};
//, "float32-filterable"

void dawnDeviceLostCallback(WGPUDevice const* device, WGPUDeviceLostReason reason, char const* message, void* userdata) {
	static const char* names[] = {"Undefined", "Unknown", "Destroyed", "InstanceDropped", "FailedCreation"};

	if (reason == WGPUDeviceLostReason_InstanceDropped) return;

	auto rname = (uint32_t)reason < std::size(names) ? names[(uint32_t)reason] : "OOPS";

	//	SGD_LOG << "Dawn device lost:" << rname;
	//	SGD_LOG << message;
	SGD_ERROR(String("Dawn device lost: ") + rname + "\n" + message);
}

void dawnErrorCallback(WGPUErrorType type, const char* message, void*) {
	static const char* names[] = {"Undefined", "NoError", "Validation", "Out of memory", "Internal", "Unknown", "Device lost"};

	auto tname = (uint32_t)type < std::size(names) ? names[(uint32_t)type] : "OOPS";

	SGD_LOG << "Dawn device error message:" << tname;
	SGD_LOG << message;
	alert(String("Dawn Device error:\n") + tname + "\n" + message);
	SGD_ABORT();
}

void dawnLoggingCallback(WGPULoggingType type, const char* message, void*) {
	static const char* names[] = {"???", "Verbose", "Info", "Warning", "Error"};

	auto tname = (uint32_t)type < std::size(names) ? names[(uint32_t)type] : "OOPS";

	switch (type) {
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
		break;
	case WGPULoggingType_Error:
		SGD_LOG << "Dawn error:" << message;
		alert(String("Dawn error: ") + message);
		SGD_ABORT();
		break;
	default:
		SGD_ERROR("Dawn logging error");
	}
}

#define LIMIT(X) SGD_LOG << #X ":" << limits.limits.X;

void logAdapterProps(const wgpu::Adapter& adapter) {

	wgpu::AdapterInfo info{};

	adapter.GetInfo(&info);
	SGD_LOG << "Dawn WGPU adapter Info:";
	SGD_LOG << "Vender name:" << (info.vendor ? info.vendor : "???");
	SGD_LOG << "Architecture:" << (info.architecture ? info.architecture : "???");
	SGD_LOG << "Name:" << (info.device ? info.device : "???");
	SGD_LOG << "Driver description:" << (info.description ? info.description : "???");
	Map<wgpu::AdapterType, String> adapterTypes{
		{wgpu::AdapterType::DiscreteGPU, "DiscreteGPU"},
		{wgpu::AdapterType::IntegratedGPU, "IntegratedGPU"},
		{wgpu::AdapterType::CPU, "CPU"},
		{wgpu::AdapterType::Unknown, "Unknown"},
	};
	auto adapterIt = adapterTypes.find(info.adapterType);
	SGD_LOG << "Adapter type:" << (adapterIt!=adapterTypes.end() ? adapterIt->second : "???");
	Map<wgpu::BackendType, String> backendTypes{
		{wgpu::BackendType::Undefined, "Undefined"}, {wgpu::BackendType::D3D12, "D3D12"},
		{wgpu::BackendType::D3D11, "D3D11"},		 {wgpu::BackendType::Vulkan, "Vulkan"},
		{wgpu::BackendType::Null, "Null"},			 {wgpu::BackendType::Metal, "Metal"},
		{wgpu::BackendType::OpenGL, "OpenGL"},		 {wgpu::BackendType::OpenGLES, "OpenGLES"},
		{wgpu::BackendType::WebGPU, "WebGPU"},
	};
	auto backendIt = backendTypes.find(info.backendType);
	SGD_LOG << "Backend type:" << (backendIt!=backendTypes.end() ? backendIt->second : "???");
	SGD_LOG << "Compatibility mode:" << info.compatibilityMode;

	SGD_LOG << "";
	SGD_LOG << "Dawn WGPU adapter supported limits:";
	wgpu::SupportedLimits limits;
	wgpuAdapterGetLimits(adapter.Get(), reinterpret_cast<WGPUSupportedLimits*>(&limits));
	LIMIT(maxTextureDimension1D);
	LIMIT(maxTextureDimension2D);
	LIMIT(maxTextureDimension3D);
	LIMIT(maxTextureArrayLayers);
	LIMIT(maxBindGroups);
	LIMIT(maxBindGroupsPlusVertexBuffers);
	LIMIT(maxBindingsPerBindGroup);
	LIMIT(maxDynamicUniformBuffersPerPipelineLayout);
	LIMIT(maxDynamicStorageBuffersPerPipelineLayout);
	LIMIT(maxSampledTexturesPerShaderStage);
	LIMIT(maxSamplersPerShaderStage);
	LIMIT(maxStorageBuffersPerShaderStage);
	LIMIT(maxStorageTexturesPerShaderStage);
	LIMIT(maxUniformBuffersPerShaderStage);
	LIMIT(maxUniformBufferBindingSize);
	LIMIT(maxStorageBufferBindingSize);
	LIMIT(minUniformBufferOffsetAlignment);
	LIMIT(minStorageBufferOffsetAlignment);
	LIMIT(maxVertexBuffers);
	LIMIT(maxBufferSize);
	LIMIT(maxVertexAttributes);
	LIMIT(maxVertexBufferArrayStride);
	LIMIT(maxInterStageShaderComponents);
	LIMIT(maxInterStageShaderVariables);
	LIMIT(maxColorAttachments);
	LIMIT(maxColorAttachmentBytesPerSample);
	LIMIT(maxComputeWorkgroupStorageSize);
	LIMIT(maxComputeInvocationsPerWorkgroup);
	LIMIT(maxComputeWorkgroupSizeX);
	LIMIT(maxComputeWorkgroupSizeY);
	LIMIT(maxComputeWorkgroupSizeZ);
	LIMIT(maxComputeWorkgroupsPerDimension);
}

} // namespace

const wgpu::Instance& getWGPUInstance() {
	static wgpu::Instance instance;
	if(instance) return instance;

	dawnProcSetProcs(&dawn::native::GetProcs());

	wgpu::InstanceDescriptor desc{};
	desc.features.timedWaitAnyEnable = true;

	return instance = wgpu::CreateInstance(&desc);
}

void requestWGPUDevice(const wgpu::RequestAdapterOptions& adapterOptions,
					   CFunction<void(const wgpu::Device& device)> callbackFunc) {

	getWGPUInstance().RequestAdapter(
		&adapterOptions,
		wgpu::CallbackMode::AllowProcessEvents,
		[=](wgpu::RequestAdapterStatus status, const wgpu::Adapter& adapter, char const* message) {
			if (status != wgpu::RequestAdapterStatus::Success) {
				SGD_ERROR("Dawn RequestAdapter() failed");
			}

			logAdapterProps(adapter);

			wgpu::DeviceDescriptor devDesc{};
			wgpu::DawnTogglesDescriptor togglesDesc{};

			Vector<const char*> enabledToggles;
			Vector<wgpu::FeatureName> requiredFeatures;

			enabledToggles.push_back("allow_unsafe_apis");
			if(!g_validationEnabled) enabledToggles.push_back("skip_validation");
			if(!g_robustnessEnabled) enabledToggles.push_back("disable_robustness");

			//for(auto s : enabledToggles) SGD_LOG << "toggle:"<<s;

			if(adapter.HasFeature(wgpu::FeatureName::TimestampQuery)) {
				requiredFeatures.push_back(wgpu::FeatureName::TimestampQuery);
			}

			devDesc.nextInChain = &togglesDesc;
			devDesc.requiredFeatureCount = requiredFeatures.size();
			devDesc.requiredFeatures = requiredFeatures.data();
			devDesc.deviceLostCallbackInfo.callback = &dawnDeviceLostCallback;
			devDesc.uncapturedErrorCallbackInfo.callback = &dawnErrorCallback;

			togglesDesc.enabledToggleCount=enabledToggles.size();
			togglesDesc.enabledToggles=enabledToggles.data();

			adapter.RequestDevice(
				&devDesc,
				wgpu::CallbackMode::AllowProcessEvents,
				[=](wgpu::RequestDeviceStatus status, const wgpu::Device& device, const char* message) {
					if (status != wgpu::RequestDeviceStatus::Success) {
						SGD_ERROR("Dawn RequestDevice() failed");
					}

					device.SetLoggingCallback(&dawnLoggingCallback, nullptr);

					callbackFunc(device);
				});
		});

	getWGPUInstance().ProcessEvents();
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
			// Can't use this, causes weird missing symbols errors on Linux!
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
