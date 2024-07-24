#pragma once

#include "keycodes.h"

#include <app/exports.h>

//! @file

struct GLFWwindow;

namespace sgd {

struct Keyboard;
struct Mouse;
struct Gamepad;

enum struct WindowFlags {
	none = 0,
	fullscreen = 1,
	resizable = 2,
	centered = 4,
	fullscreen60hz = 8,
};

struct Window : Shared {
	SGD_OBJECT_TYPE(Window, Shared);

	static constexpr uint32_t maxGamepads = 4;

	Signal<Vec2u> sizeChanged0;
	Signal<Vec2u> sizeChanged1;
	Signal<Vec2u> sizeChanged2;
	Signal<Vec2u> sizeChanged;
	Signal<> lostFocus;
	Signal<> gotFocus;
	Signal<> closeClicked;

	Window(CVec2u size, CString title, WindowFlags flags);
	~Window() override;

	CVec2u size() const {
		return m_size;
	}

	WindowFlags flags() const {
		return m_flags;
	}

	bool hasFocus() const {
		return m_hasFocus;
	}

	Keyboard* keyboard()const {
		return m_keyboard;
	}

	Mouse* mouse() const {
		return m_mouse;
	}

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
	bool m_hasFocus;

	Keyboard* m_keyboard;
	Mouse* m_mouse;
};

} // namespace sgd
