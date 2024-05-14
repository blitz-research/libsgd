#pragma once

#include "uidevice.h"
#include "keycodes.h"

struct GLFWwindow;

namespace sgd {

class Keyboard : public UIDevice {
public:
	static constexpr uint32_t maxButtons = 512;

	explicit Keyboard(GLFWwindow* window);

	Deque<uint32_t>& charQueue() {
		return m_charQueue;
	}

	Button& key(KeyCode keyCode) const {
		return button((uint32_t)keyCode);
	}

private:
	Deque<uint32_t> m_charQueue;
};

} // namespace sgd
