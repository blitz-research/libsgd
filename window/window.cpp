#include "window.h"

#include "gamepad.h"
#include "keyboard.h"
#include "mouse.h"

#include <GLFW/glfw3.h>

#if SGD_OS_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32 1
#define GLFW_EXPOSE_NATIVE_WGL 1
#define WIN32_LEAN_AND_MEAN

#include <GLFW/glfw3native.h>

#undef min
#undef max

#elif SGD_OS_LINUX
#define GLFW_EXPOSE_NATIVE_X11 1
#define GLFW_EXPOSE_NATIVE_GLX 1
#include <GLFW/glfw3native.h>
#undef Success
#undef Always
#undef None

#elif SGD_OS_EMSCRIPTEN
#include <emscripten.h>

namespace {

EM_JS(void, getCanvasClientSize, (uint32_t * width, uint32_t* height), {
	var rect = Model.canvas.getBoundingClientRect();
	HEAP32[width >> 2] = rect.width;
	HEAP32[height >> 2] = rect.height;
});

EM_JS(void, getCanvasBufferSize, (uint32_t * width, uint32_t* height), {
	HEAP32[width >> 2] = Module.canvas.width;
	HEAP32[height >> 2] = Module.canvas.height;
});

} // namespace

#endif

namespace sgd {

Window::Window(CVec2u size, CString title, WindowFlags flags) : m_flags(flags) {

	runOnMainThread(
		[=] {
			initApp();

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

			GLFWmonitor* monitor{};
			if (bool(flags & WindowFlags::fullscreen)) {
				monitor = glfwGetPrimaryMonitor();
				m_fullscreenSize = size;
				m_fullscreenHz = 0;
			} else if (bool(flags & WindowFlags::fullscreen60hz)) {
				monitor = glfwGetPrimaryMonitor();
				glfwWindowHint(GLFW_REFRESH_RATE, 60);
				m_fullscreenSize = size;
				m_fullscreenHz = 60;
			} else {
				glfwWindowHint(GLFW_RESIZABLE, bool(m_flags & WindowFlags::resizable));
				//				glfwWindowHint(GLFW_DECORATED, bool(m_flags & WindowFlags::resizable));
				if (bool(flags & WindowFlags::centered)) {
					auto pos = (Vec2i(desktopSize()) - Vec2i(size)) / 2;
					glfwWindowHint(GLFW_POSITION_X, pos.x);
					glfwWindowHint(GLFW_POSITION_Y, pos.y);
				}
				m_fullscreenSize = desktopSize();
				m_fullscreenHz = 0;
			}

			m_title = title;
			m_flags = flags;
			m_glfwWindow = glfwCreateWindow((int)size.x, (int)size.y, m_title.c_str(), monitor, nullptr);
			glfwSetWindowUserPointer(m_glfwWindow, this);

			glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int w, int h) { //
				auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
				Vec2u size{(uint32_t)w, (uint32_t)h};
				if (size != window->m_size) {
					if (w && h) {
						if (!window->m_size.x || !window->m_size.y) resumeApp();
					} else {
						if (window->m_size.x && window->m_size.y) suspendApp();
					}
					window->m_size = size;
					window->sizeChanged0.emit(window->m_size);
					window->sizeChanged.emit(window->m_size);
				}
				window->updateState();
			});
			uint32_t w, h;
			glfwGetWindowSize(m_glfwWindow, (int*)&w, (int*)&h);
			m_size = {w, h};

			glfwSetWindowPosCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int x, int y) { //
				auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
				Vec2i position{x, y};
				if (position != window->m_position) {
					window->m_position = position;
					window->positionChanged.emit(position);
				}
				//				window->updateState();
			});
			int x, y;
			glfwGetWindowPos(m_glfwWindow, &x, &y);
			m_position = {x, y};

		// Note: We don't use these because they're called *before* size callback.
		// Rely on size callback to update state instead.
#if 0
			glfwSetWindowMaximizeCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int maximized) { //
				auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
				window->updateState();
			});

			glfwSetWindowIconifyCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int iconified) { //
				auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
				window->updateState();
			});
#endif
			glfwSetWindowFocusCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int focused) {
				auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
				if ((bool)focused == window->m_hasFocus) return;
				(window->m_hasFocus = focused) ? window->gotFocus.emit() : window->lostFocus.emit();
			});
			m_hasFocus = glfwGetWindowAttrib(m_glfwWindow, GLFW_FOCUSED);

			glfwSetWindowCloseCallback(m_glfwWindow, [](GLFWwindow* glfwWindow) {
				auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
				window->closeClicked.emit();
			});

			m_keyboard = new Keyboard(m_glfwWindow);

			m_mouse = new Mouse(m_glfwWindow);

			updateState();
		},
		true);
	SGD_ASSERT(m_glfwWindow);
}

Window::~Window() {
	close();
}

void Window::close() {
	runOnMainThread(
		[=] {
			if (!m_glfwWindow) return;
			glfwDestroyWindow(m_glfwWindow);
			m_glfwWindow = nullptr;
		},
		true);
}

void Window::setTitle(CString title) {
	runOnMainThread(
		[=]() {
			if (title == m_title) return;
			m_title = title;
			glfwSetWindowTitle(m_glfwWindow, m_title.c_str());
		},
		true);
}

void Window::setPosition(CVec2i position) {
	runOnMainThread(
		[=]() {
			if (position == m_position) return;
			glfwSetWindowPos(m_glfwWindow, position.x, position.y);
		},
		true);
}

void Window::setSize(CVec2u size) {
	runOnMainThread(
		[=]() {
			if (size == m_size) return;
			glfwSetWindowSize(m_glfwWindow, (int)size.x, (int)size.y);
		},
		true);
}

void Window::setFullscreenMode(CVec2u size, uint32_t hz) {
	runOnMainThread(
		[=]() {
			m_desktopPosition = m_position;
			m_desktopSize = m_size;
			m_fullscreenSize = size;
			m_fullscreenHz = hz;
			glfwSetWindowMonitor(m_glfwWindow, glfwGetPrimaryMonitor(), 0,0, //
								 (int)m_fullscreenSize.x, (int)m_fullscreenSize.y, m_fullscreenHz ? (int) m_fullscreenHz : GLFW_DONT_CARE);
		},
		true);
}

void Window::setState(WindowState state) {
	runOnMainThread(
		[=]() {
			switch (state) {
			case WindowState::fullscreen:
				setFullscreenMode(m_fullscreenSize, m_fullscreenHz);
				break;
			case WindowState::maximized:
				glfwMaximizeWindow(m_glfwWindow);
				break;
			case WindowState::normal:
				if (m_state == WindowState::fullscreen) {
					glfwSetWindowMonitor(m_glfwWindow, nullptr, m_desktopPosition.x, m_desktopPosition.y, //
										 (int)m_desktopSize.x, (int)m_desktopSize.y, GLFW_DONT_CARE);
				} else {
					glfwRestoreWindow(m_glfwWindow);
				}
				break;
			case WindowState::minimized:
				glfwIconifyWindow(m_glfwWindow);
				break;
			case WindowState::closed:
				return;
			default:
				SGD_ERROR("Invalid WindowState");
			}
		},
		true);
}

void Window::updateState() {
	auto state = WindowState::closed;
	if (m_glfwWindow) {
		if (glfwGetWindowMonitor(m_glfwWindow)) {
			state = WindowState::fullscreen;
		} else if (glfwGetWindowAttrib(m_glfwWindow, GLFW_MAXIMIZED)) {
			state = WindowState::maximized;
		} else if (glfwGetWindowAttrib(m_glfwWindow, GLFW_ICONIFIED)) {
			state = WindowState::minimized;
		} else {
			state = WindowState::normal;
		}
	}
	if (state == m_state) return;
	m_state = state;
	stateChanged.emit(m_state);
}

Window* Window::getWindow(GLFWwindow* glfwWindow) {
	return static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
}

} // namespace sgd
