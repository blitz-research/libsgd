#include "window.h"

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
			if (!glfwInit()) SGD_ABORT();

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, bool(m_flags & WindowFlags::resizable));

			auto monitor = bool(flags & WindowFlags::fullscreen) ? glfwGetPrimaryMonitor() : nullptr;

			m_glfwWindow = glfwCreateWindow(size.x, size.y, title.c_str(), monitor, nullptr);
			glfwSetWindowUserPointer(m_glfwWindow, this);
			{
				glfwSetWindowSizeCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, int w, int h) {
					auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
					window->m_size = Vec2u(w, h);
					window->sizeChanged.emit(window->m_size);
				});
				int w, h;
				glfwGetWindowSize(m_glfwWindow, &w, &h);
				m_size = Vec2u(w, h);
			}
			{
				glfwSetCursorPosCallback(m_glfwWindow, [](GLFWwindow* glfwWindow, double x, double y) {
					auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
					window->m_cursorPos = Vec2f(x, y);
					window->cursorPosChanged.emit(window->m_cursorPos);
				});
				double x, y;
				glfwGetCursorPos(m_glfwWindow, &x, &y);
				m_cursorPos = Vec2f(x, y);
			}
			{
				glfwSetWindowCloseCallback(m_glfwWindow, [](GLFWwindow* glfwWindow) {
					auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
					window->closeClicked.emit();
				});
			}
		},
		true);
	SGD_ASSERT(m_glfwWindow);
}

Window::~Window() {
	runOnMainThread(
		[=] {
			//SGD_ASSERT(!m_glfwWindow);
			close();
		},
		true);
}

bool Window::pollEvents() {
	bool result{};

	runOnMainThread(
		[=, &result] {
#if SGD_OS_EMSCRIPTEN
			Vec2u size;
			getCanvasBufferSize(&size.x, &size.y);
			if (size != m_size) sizeChanged.emit((m_size = size));
#endif
			glfwPollEvents();
			result = !glfwWindowShouldClose(m_glfwWindow);
		},
		true);

	return result;
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

} // namespace sgd
