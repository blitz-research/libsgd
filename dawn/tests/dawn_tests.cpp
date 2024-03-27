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

GLFWwindow* window;

wgpu::Device device;
wgpu::Surface surface;
wgpu::SwapChain swapChain;

void render() {
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

	window = glfwCreateWindow(width, height, "Dawn test", nullptr, nullptr);

	device = createWGPUDevice({});
	surface = createWGPUSurface(device, window);
	swapChain = createWGPUSwapChain(device, surface, {width, height}, preferredWGPUSwapChainFormat(device));

	glfwSetWindowSizeCallback(window, [](GLFWwindow*, int x, int y) {
		swapChain = createWGPUSwapChain(device, surface, {(uint32_t)x, (uint32_t)y}, preferredWGPUSwapChainFormat(device));
	});

	for (;;) {
		glfwPollEvents();
		if (glfwWindowShouldClose(window)) std::exit(0);

		render();
	}
}
