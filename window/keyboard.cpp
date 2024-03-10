#include "keyboard.h"

#include "window.h"

#include <GLFW/glfw3.h>

namespace sgd {

Keyboard::Keyboard(GLFWwindow* glfwWindow) : UIDevice(maxButtons) {

	glfwSetKeyCallback(glfwWindow, [](GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
		if (key >= maxButtons || (action != GLFW_PRESS && action != GLFW_RELEASE)) return;

		auto keyboard = Window::getWindow(glfwWindow)->keyboard();
		keyboard->setButtonDown(key, action == GLFW_PRESS);
	});

	glfwSetCharCallback(glfwWindow, [](GLFWwindow* glfwWindow, uint32_t chr) {
		auto keyboard = Window::getWindow(glfwWindow)->keyboard();
		if(keyboard->m_charQueue.size()==256) return;

		keyboard->m_charQueue.push_back(chr);
	});
}

} // namespace sgd
