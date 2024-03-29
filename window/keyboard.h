#pragma once

#include "uidevice.h"

struct GLFWwindow;

namespace sgd {

class Keyboard : public UIDevice {
public:
	static constexpr uint32_t maxButtons = 512;

	explicit Keyboard(GLFWwindow* window);

	Deque<uint32_t>& charQueue() {
		return m_charQueue;
	}

	Button& key(uint32_t index) const {
		return button(index);
	}

private:
	Deque<uint32_t> m_charQueue;
};

} // namespace sgd
