#pragma once

#include "uidevice.h"

struct GLFWwindow;

namespace sgd {

enum struct CursorMode { undefined, normal, hidden, disabled, captured };

class Mouse : public UIDevice {
public:
	static constexpr uint32_t maxButtons = 3;

	explicit Mouse(GLFWwindow* window);

	Property<CursorMode> cursorMode;

	Vec3f position() const {
		return m_position;
	}

	Vec3f velocity() const {
		return m_velocity;
	}

	void setPositionZ(float z);

private:
	GLFWwindow* m_window;
	Vec3f m_position;
	Vec3f m_velocity;
};

} // namespace sgd
