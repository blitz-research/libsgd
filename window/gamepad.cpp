#include "gamepad.h"

#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

namespace sgd {

namespace {

Gamepad* g_gamepads[Gamepad::maxGamepads];

}

// ***** Gamepad *****

Gamepad* Gamepad::getGamepad(uint32_t id) {
	SGD_ASSERT(id < maxGamepads);

	return g_gamepads[id];
}

void Gamepad::createGamepads() {
	SGD_ASSERT(!g_gamepads[0]);

	for (int id = 0; id < maxGamepads; ++id) {
		g_gamepads[id] = new Gamepad(id);
		if (glfwJoystickPresent(id)) g_gamepads[id]->onConnected();
	}

	glfwSetJoystickCallback([](int id, int event) {
		if ((uint32_t)id >= maxGamepads) return;
		if (event == GLFW_CONNECTED) {
			g_gamepads[id]->onConnected();
		} else if (event == GLFW_DISCONNECTED) {
			g_gamepads[id]->onDisconnected();
		}
	});
}

void Gamepad::updateGamepads() {

#ifdef __EMSCRIPTEN__
	emscripten_sample_gamepad_data();
#endif

	for (int id = 0; id < maxGamepads; ++id) g_gamepads[id]->update();
}

Gamepad::Gamepad(uint32_t id) : UIDevice(numButtons), m_id(id) {
}

void Gamepad::onConnected() {
	SGD_ASSERT(!m_connected);
	if (!glfwJoystickIsGamepad(m_id)) return;

	m_connected = true;
#ifndef __EMSCRIPTEN__
	auto p = glfwGetGamepadName(m_id);
	m_name = p ? String(p) : String();
#endif

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

void Gamepad::onPoll() {
	if (!m_connected) return;

#ifndef __EMSCRIPTEN__
	GLFWgamepadstate state;
	glfwGetGamepadState(m_id, &state);

	for (int i = 0; i < numButtons; ++i) setButtonValue(i, state.buttons[i]);

	std::memcpy(m_axes, state.axes, sizeof(m_axes));
#endif
}

} // namespace sgd
