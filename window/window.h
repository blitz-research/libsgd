#pragma once

#include "keycodes.h"

#include <app/exports.h>

//! @file

struct GLFWwindow;

namespace sgd {

struct Keyboard;
struct Mouse;
struct Gamepad;

enum struct WindowState {
	closed = 0,
	minimized = 1,
	normal = 2,
	maximized = 3,
	fullscreen = 4,
};

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

	Signal<Vec2i> positionChanged;
	Signal<Vec2u> sizeChanged0;
	Signal<Vec2u> sizeChanged;
	Signal<WindowState> stateChanged;

	Signal<> lostFocus;
	Signal<> gotFocus;
	Signal<> closeClicked;

	Window(CVec2u size, CString title, WindowFlags flags);
	~Window() override;

	void setTitle(CString title);
	CString title() const {
		return m_title;
	}

	void setPosition(CVec2i position);
	CVec2i position() const {
		return m_position;
	}

	void setSize(CVec2u size);
	CVec2u size() const {
		return m_size;
	}

	void setFullscreenMode(CVec2u size, uint32_t hz);

	void setState(WindowState state);
	WindowState state() const {
		return m_state;
	}

	WindowFlags flags() const {
		return m_flags;
	}

	bool hasFocus() const {
		return m_hasFocus;
	}

	Keyboard* keyboard() const {
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
	String m_title;
	WindowFlags m_flags;
	GLFWwindow* m_glfwWindow{};
	Vec2u m_size;
	uint32_t m_hertz;
	Vec2i m_position;
	bool m_hasFocus;
	WindowState m_state{WindowState::closed};

	Vec2u m_fullscreenSize{1920, 1080};
	uint32_t m_fullscreenHz{60};
	Vec2i m_desktopPosition;
	Vec2u m_desktopSize;;

	Keyboard* m_keyboard;
	Mouse* m_mouse;

	void updateState();
};

} // namespace sgd
