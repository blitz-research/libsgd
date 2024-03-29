#include "mouse.h"

#include "window.h"

#include <GLFW/glfw3.h>

namespace sgd {

Mouse::Mouse(GLFWwindow* window) : UIDevice(maxButtons) {

	glfwSetMouseButtonCallback(window, [](GLFWwindow* glfwWindow, int button, int action, int mods) {
		if (button >= maxButtons) return;

		auto mouse = Window::getWindow(glfwWindow)->mouse();
		mouse->setButtonDown(button, action == GLFW_PRESS);
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* glfwWindow, double x, double y) {
		auto mouse = Window::getWindow(glfwWindow)->mouse();
		mouse->m_position = {float(x), float(y)};
	});

	glfwSetScrollCallback(window, [](GLFWwindow* glfwWindow, double x, double y) {
		auto mouse = Window::getWindow(glfwWindow)->mouse();
		mouse->m_scroll = {float(x), float(y)};
	});
}

} // namespace sgd
