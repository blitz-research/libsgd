#pragma once

#include "uidevice.h"

struct GLFWwindow;

namespace sgd {

enum struct CursorMode { undefined, normal, hidden, disabled, captured };

class Mouse : public UIDevice {
public:
	static constexpr uint32_t maxButtons = 3;

	Signal<Vec2f> positionChanged;
	Signal<Vec2f> scrollChanged;

	explicit Mouse(GLFWwindow* window);

	Property<CursorMode> cursorMode;

	Vec2f position() const {
		return m_position;
	}

	Vec2f velocity() const {
		return m_velocity;
	}

	Vec2f scroll() const {
		return m_scroll;
	}

private:
	GLFWwindow* m_window;
	Vec2f m_position;
	Vec2f m_velocity;
	Vec2f m_scroll;
};

} // namespace sgd
