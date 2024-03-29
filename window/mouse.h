#pragma once

#include "uidevice.h"

struct GLFWwindow;

namespace sgd {

class Mouse : public UIDevice {
public:
	static constexpr uint32_t maxButtons = 3;

	Mouse(GLFWwindow* window);

	Vec2f position() const {
		return m_position;
	}

	Vec2f scroll() const {
		return m_scroll;
	}

private:
	Vec2f m_position;
	Vec2f m_scroll;
};

} // namespace sgf
