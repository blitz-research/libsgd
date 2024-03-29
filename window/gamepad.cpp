#include "gamepad.h"

#include "window.h"

#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

namespace sgd {

namespace {

Gamepad* g_gamepads[Gamepad::maxGamepads];

}

// ***** Gamepad *****

Gamepad::Gamepad(uint32_t id) : UIDevice(numButtons), m_id(id) {
	endPollEvents.connect(this, [=] { poll(); });\
}

Gamepad* Gamepad::getGamepad(uint32_t id) {
	SGD_ASSERT(id < maxGamepads);

	if (g_gamepads[id]) return g_gamepads[id];

	for (int i = 0; i < maxGamepads; ++i) {
		g_gamepads[i] = new Gamepad(i);

		if (glfwJoystickPresent(i)) g_gamepads[i]->onConnected();
	}

	glfwSetJoystickCallback([](int id, int event) {
		if ((uint32_t)id >= maxGamepads) return;
		if (event == GLFW_CONNECTED) {
			g_gamepads[id]->onConnected();
		} else if (event == GLFW_DISCONNECTED) {
			g_gamepads[id]->onDisconnected();
		}
	});

	return g_gamepads[id];
}

void Gamepad::onConnected() {
	SGD_ASSERT(!m_connected);
	if (!glfwJoystickIsGamepad((int)m_id)) return;

	m_connected = true;
#ifndef __EMSCRIPTEN__
	auto p = glfwGetGamepadName((int)m_id);
	m_name = p ? String(p) : String();
#endif

	poll();

	log() << "### Gamepad" << m_id << m_name << "connected.";
}

void Gamepad::onDisconnected() {
	if (!m_connected) return;

	m_connected = false;
	memset(m_axes, 0, sizeof(m_axes));
	m_name = "";
	flush();

	log() << "### Gamepad" << m_id << "disconnected.";
}

void Gamepad::poll() {
	if (!m_connected) return;

#ifndef __EMSCRIPTEN__
	GLFWgamepadstate state;
	glfwGetGamepadState((int)m_id, &state);

	for (int i = 0; i < numButtons; ++i) setButtonDown(i, state.buttons[i]);

	std::memcpy(m_axes, state.axes, sizeof(m_axes));
#endif
}

} // namespace sgd
