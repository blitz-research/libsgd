#pragma once

#include "keycodes.h"

#include <geom/exports.h>

//! @file

struct GLFWwindow;

namespace sgd {

struct Keyboard;
struct Mouse;
struct Gamepad;

enum struct WindowFlags {
	none = 0,
	fullscreen = 1,
	resizable = 2
};

struct Window : Shared {
	SGD_OBJECT_TYPE(Window, Shared);

	static constexpr uint32_t maxGamepads = 4;

	Signal<Vec2u> sizeChanged;
	Signal<> closeClicked;

	Window(CVec2u size, CString title, WindowFlags flags);
	~Window() override;

	CVec2u size() const {
		return m_size;
	}

	WindowFlags flags() const {
		return m_flags;
	}

	Keyboard* keyboard()const {
		return m_keyboard;
	}

	Mouse* mouse() const {
		return m_mouse;
	}

	bool pollEvents();

	void close();

	// ***** Internal *****

	static Window* getWindow(GLFWwindow* glfwWindow);

	GLFWwindow* glfwWindow() const {
		return m_glfwWindow;
	}

private:
	GLFWwindow* m_glfwWindow{};

	WindowFlags m_flags;

	Vec2u m_size;
	Vec2f m_mousePos;

	Keyboard* m_keyboard;
	Mouse* m_mouse;
};

} // namespace sgd
