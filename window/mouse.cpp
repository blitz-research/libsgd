#include "mouse.h"

#include "window.h"

#include <GLFW/glfw3.h>

namespace sgd {

auto mode = GLFW_CURSOR_NORMAL;

Mouse::Mouse(GLFWwindow* window) : UIDevice(maxButtons), m_window(window) {

	beginPollEvents.connect(this, [=]{
		m_velocity = {};
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* glfwWindow, double x, double y) { //
		auto mouse = Window::getWindow(glfwWindow)->mouse();
		Vec2f position{(float)x, (float)y};
		mouse->m_velocity += (position - mouse->m_position);	// += in case we get multiple callbacks?
		mouse->m_position = position;
	});

	glfwSetMouseButtonCallback(window, [](GLFWwindow* glfwWindow, int button, int action, int mods) {
		if (button >= maxButtons) return;

		auto mouse = Window::getWindow(glfwWindow)->mouse();
		mouse->setButtonDown(button, action == GLFW_PRESS);
	});

	glfwSetScrollCallback(window, [](GLFWwindow* glfwWindow, double x, double y) { //
		auto mouse = Window::getWindow(glfwWindow)->mouse();
		mouse->m_scroll = {float(x), float(y)};
	});

	cursorMode.changed.connect(this, [=](CursorMode mode) { //
		m_velocity = {};
		runOnMainThread([=]{
			glfwSetInputMode(m_window, GLFW_CURSOR, (int)mode + 0x34000);
		}, true);
	});
}

} // namespace sgd
