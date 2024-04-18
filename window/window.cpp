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

namespace {

int g_suspended;

void suspendApp() {
	if (++g_suspended == 1) {
		log() << "### App suspended";
		appSuspended.emit();
	}
}

void resumeApp() {
	if (--g_suspended == 0) {
		log() << "### App resumed";
		appResumed.emit();
	}
}

} // namespace

Window::Window(CVec2u size, CString title, WindowFlags flags) : m_flags(flags) {
	runOnMainThread(
		[=] {
			if (!glfwInit()) SGD_ABORT();

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, bool(m_flags & WindowFlags::resizable));

			int w=(int)size.x, h=(int)size.y;

			GLFWmonitor* monitor{};
			if(bool(flags & WindowFlags::fullscreen)) {
				monitor = glfwGetPrimaryMonitor();
				if(bool(flags & WindowFlags::rgba8_60hz)) {
					glfwWindowHint(GLFW_RED_BITS, 8);
					glfwWindowHint(GLFW_GREEN_BITS, 8);
					glfwWindowHint(GLFW_BLUE_BITS, 8);
					glfwWindowHint(GLFW_REFRESH_RATE, 60);
				} else{
					const GLFWvidmode* mode = glfwGetVideoMode(monitor);
 					glfwWindowHint(GLFW_RED_BITS, mode->redBits);
					glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
					glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
					glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
					w = mode->width;
					h = mode->height;
				}
			}

			m_glfwWindow = glfwCreateWindow(w, h, title.c_str(), monitor, nullptr);

			glfwSetWindowUserPointer(m_glfwWindow, this);

			{
				glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int w, int h) {
					auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
					if(window->m_size == Vec2u(w,h)) return;
					if (w && h) {
						if (!window->m_size.x || !window->m_size.y) resumeApp();
					} else {
						if (window->m_size.x && window->m_size.y) suspendApp();
					}
					window->m_size = Vec2u(w, h);
					window->sizeChanged0.emit(window->m_size);
					window->sizeChanged1.emit(window->m_size);
					window->sizeChanged2.emit(window->m_size);
					window->sizeChanged.emit(window->m_size);
				});
				int w, h;
				glfwGetWindowSize(m_glfwWindow, &w, &h);
				g_suspended = !w || !h;
				m_size = Vec2u(w, h);
			}
			{
				glfwSetWindowFocusCallback(m_glfwWindow, [](GLFWwindow * glfwWindow, int focused){
					auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
					if((bool)focused == window->m_hasFocus) return;
					(window->m_hasFocus = focused) ? window->gotFocus.emit() : window->lostFocus.emit();
				});
				m_hasFocus = glfwGetWindowAttrib(m_glfwWindow, GLFW_FOCUSED);
			}
			{
				glfwSetWindowCloseCallback(m_glfwWindow, [](GLFWwindow* glfwWindow) {
					auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
					window->closeClicked.emit();
				});
			}

			m_keyboard = new Keyboard(m_glfwWindow);

			m_mouse = new Mouse(m_glfwWindow);
		},
		true);
	SGD_ASSERT(m_glfwWindow);
}

Window::~Window() {
	if (m_glfwWindow) close();
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

Window* Window::getWindow(GLFWwindow* glfwWindow) {
	return static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
}

void pollEvents() {

	// This is to delay glfwWaitEvents by 1 poll, so user can 'see' suspend events.
	static bool wait;

	runOnMainThread(
		[] { //
			beginPollEvents.emit();
			if (g_suspended && wait) {
				glfwWaitEvents();
			} else {
				glfwPollEvents();
			}
			wait = g_suspended;
			endPollEvents.emit();
		},
		true);
}

} // namespace sgd
