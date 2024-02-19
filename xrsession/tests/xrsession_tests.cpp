#include <openxr/exports.h>

#include <GLFW/glfw3.h>

using namespace sgd;

int main() {

	uint32_t width = 640;
	uint32_t height = 480;

	if (!glfwInit()) SGD_ABORT();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#if SGD_OS_MACOS
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
	auto window = glfwCreateWindow(width, height, "OpenXR test", nullptr, nullptr);
	SGD_ASSERT(window);

	auto device = createXRCompatibleWGPUDevice();
	auto session = createXRSession(device);

	auto surface = createWGPUSurface(device, window);
	auto swapChain = createWGPUSwapChain(device, surface, {width, height}, preferredWGPUSwapChainFormat(device));

	for(;;) {
		glfwPollEvents();
		if(glfwWindowShouldClose(window)) std::exit(0);

		auto frame = session->beginFrame();

		drawHelloTriangle(device, frame.swapChainImage);

		session->endFrame();

		copyTexture(device, frame.swapChainImage, swapChain.GetCurrentTexture());
		swapChain.Present();
	}
}
