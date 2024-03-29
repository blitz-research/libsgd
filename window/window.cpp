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

template <class FunTy, class RetTy = typename std::invoke_result_t<FunTy>> //
RetTy runSync(FunTy fun) {

	RetTy result;
	CondVar<bool> done;

	runOnMainThread([&]{
		result=fun();
		done.set(true);
	}, true);

	done.waiteq(true);

	return 0;
}

void pollEvents() {

	runSync([]() -> int { //
		return 0;
	});

	runOnMainThread(
		[] { //
			beginPollEvents.emit();
			glfwPollEvents();
			endPollEvents.emit();
		},
		true);
}

} // namespace sgd
