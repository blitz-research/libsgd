#pragma once

#include <geom/exports.h>

//! @file

struct GLFWwindow;

namespace sgd {

enum struct WindowFlags {
	none = 0,
	fullscreen = 1,
	resizable = 2
};

struct Window : Shared {
	SGD_OBJECT_TYPE(Window, Shared);

	Signal<Vec2u> sizeChanged;
	Signal<Vec2f> cursorPosChanged;
	Signal<> closeClicked;

	using RenderFunc = Function<void(Window*)>;

	Window(CVec2u size, CString title, WindowFlags flags);
	~Window();

	CVec2u size() const {
		return m_size;
	}

	WindowFlags flags() const {
		return m_flags;
	}

	CVec2f cursorPos() const {
		return m_cursorPos;
	}

	bool pollEvents();

	void close();

	// ***** Internal *****

	GLFWwindow* glfwWindow() const {
		return m_glfwWindow;
	}

	void* nativeWindow()const;

private:
	GLFWwindow* m_glfwWindow{};

	WindowFlags m_flags;

	Vec2u m_size;
	Vec2f m_cursorPos;
};

} // namespace sgd
