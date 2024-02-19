#include <dawn/exports.h>

#include <GLFW/glfw3.h>

#if SGD_OS_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32 1
#define GLFW_EXPOSE_NATIVE_WGL 1
#define WIN32_LEAN_AND_MEAN
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

using namespace sgd;

wgpu::Device device;
wgpu::Surface surface;
wgpu::SwapChain swapChain;

void render() {
	glfwPollEvents();

	drawHelloTriangle(device, swapChain.GetCurrentTexture());

#if !SGD_OS_EMSCRIPTEN
	swapChain.Present();
#endif
}

int main() {

	uint32_t width = 640;
	uint32_t height = 480;

	if (!glfwInit()) SGD_ABORT();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

#if SGD_OS_MACOS
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
	auto window = glfwCreateWindow(width, height, "Dawn test", nullptr, nullptr);
	SGD_ASSERT(window);

	void* nativeWindow;
#if SGD_OS_WINDOWS
	nativeWindow = (void*)glfwGetWin32Window(window);
#elif SGD_OS_LINUX
	SGD_ABORT();
#elif SGD_OS_EMSCRIPTEN
	nativeWindow = (void*)"#canvas";
#endif

	requestWGPUDevice({}, [=](const wgpu::Device& result) {
		device = result;
		surface = createWGPUSurface(device, nativeWindow);
		swapChain = createWGPUSwapChain(device, surface, {width, height}, preferredWGPUSwapChainFormat(device));
		requestRender(render);
	});

	beginAppEventLoop();
}
