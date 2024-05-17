#include <dawn/exports.h>

#include <cassert>

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

void render() {
	struct wgpu::SurfaceTexture surfaceTexture {};
	surface.GetCurrentTexture(&surfaceTexture);
	assert(surfaceTexture.status == wgpu::SurfaceGetCurrentTextureStatus::Success);
	drawHelloTriangle(device, surfaceTexture.texture);
	surface.Present();
}

int main() {

	uint32_t width = 640;
	uint32_t height = 480;

	if (!glfwInit()) SGD_ABORT();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#if SGD_OS_MACOS
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif

	window = glfwCreateWindow((int)width, (int)height, "Dawn test", nullptr, nullptr);

	device = createWGPUDevice({});
	surface = createWGPUSurface(device, window);

	static auto configureSurface = [=](CVec2u size) {
		wgpu::SurfaceConfiguration config{};
		config.device = device;
		config.width = width;
		config.height = height;
		config.format = surface.GetPreferredFormat(device.GetAdapter());
		config.usage = wgpu::TextureUsage::RenderAttachment;
		config.presentMode = wgpu::PresentMode::Fifo;
		config.viewFormatCount = 0;
		config.viewFormats = nullptr;
		config.alphaMode = wgpu::CompositeAlphaMode::Opaque;
		surface.Configure(&config);
	};

	glfwSetWindowSizeCallback(window, [](GLFWwindow*, int x, int y) {
		configureSurface(Vec2u(x,y));
	});

	configureSurface(Vec2u(width,height));

	for (;;) {
		glfwPollEvents();
		if (glfwWindowShouldClose(window)) std::exit(0);

		render();
	}
}
