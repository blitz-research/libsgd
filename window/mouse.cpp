#include "mouse.h"

#include "window.h"

#include <GLFW/glfw3.h>

namespace sgd {

Mouse::Mouse(GLFWwindow* glfwWindow) : UIDevice(maxButtons) {

	glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* glfwWindow, double x, double y) {
		auto mouse = Window::getWindow(glfwWindow)->mouse();
		mouse->m_position = {float(x), float(y)};
	});

	glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* glfwWindow, int button, int action, int mods) {
		if(button>=maxButtons) return;

		auto mouse = Window::getWindow(glfwWindow)->mouse();
		mouse->setButtonDown(button, action == GLFW_PRESS);
	});

	glfwSetScrollCallback(glfwWindow, [](GLFWwindow* glfwWindow, double x, double y) {
		auto mouse = Window::getWindow(glfwWindow)->mouse();
		mouse->m_scroll = {float(x), float(y)};
	});
}

} // namespace sgf
