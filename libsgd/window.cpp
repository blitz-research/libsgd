#include "internal.h"

void SGD_DECL sgd_CreateWindow(int width, int height, SGD_String title, int flags) {
	if (sgdx::g_mainWindow) sgd_Error("Window has already been created");
	sgdx::started();

	auto window = new sgd::Window(sgdx::Vec2u(width, height), title, (sgdx::WindowFlags)flags);

	window->closeClicked.connect(nullptr, [] { //
		sgdx::postEvent({SGD_EVENT_MASK_CLOSE_CLICKED});
	});

	window->sizeChanged.connect(nullptr, [](sgdx::CVec2u size) { //
		if (sgdx::g_mainScene) {
			sgd_RenderScene();
			sgd_Present();
		}
		sgdx::postEvent({SGD_EVENT_MASK_SIZE_CHANGED});
	});

	window->lostFocus.connect(nullptr, [] { //
		sgdx::postEvent({SGD_EVENT_MASK_LOST_FOCUS});
	});

	window->gotFocus.connect(nullptr, [] { //
		sgdx::postEvent({SGD_EVENT_MASK_GOT_FOCUS});
	});

	sgdx::g_mainWindow = window;
}

void SGD_DECL sgd_DestroyWindow() {
	delete sgdx::mainWindow();
	sgdx::g_mainWindow = nullptr;
}

int SGD_DECL sgd_WindowWidth() {
	return (int)sgdx::mainWindow()->size().x;
}

int SGD_DECL sgd_WindowHeight() {
	return (int)sgdx::mainWindow()->size().y;
}

SGD_Bool SGD_DECL sgd_KeyDown(int keyCode) {
	if ((uint32_t)keyCode > SGD_KEY_LAST) sgd_Error("Key code out of range");

	return sgdx::mainWindow()->keyboard()->key(keyCode).down();
}

SGD_Bool SGD_DECL sgd_KeyHit(int keyCode) {
	if ((uint32_t)keyCode > SGD_KEY_LAST) sgd_Error("Key code out of range");

	return sgdx::mainWindow()->keyboard()->key(keyCode).hit();
}

int SGD_DECL sgd_GetChar() {
	auto& queue = sgdx::mainWindow()->keyboard()->charQueue();
	if (queue.empty()) return 0;

	auto chr = queue.front();
	queue.pop_front();
	return (int)chr;
}

float SGD_DECL sgd_MouseX() {
	return sgdx::mainWindow()->mouse()->position().x;
}

float SGD_DECL sgd_MouseY() {
	return sgdx::mainWindow()->mouse()->position().y;
}

float SGD_DECL sgd_MouseVX() {
	return sgdx::mainWindow()->mouse()->velocity().x;
}

float SGD_DECL sgd_MouseVY() {
	return sgdx::mainWindow()->mouse()->velocity().y;
}

float SGD_DECL sgd_MouseScrollX() {
	return sgdx::mainWindow()->mouse()->scroll().x;
}

float SGD_DECL sgd_MouseScrollY() {
	return sgdx::mainWindow()->mouse()->scroll().y;
}

void SGD_DECL sgd_SetCursorMode(int cursorMode) {
	sgdx::mainWindow()->mouse()->cursorMode = (sgd::CursorMode)cursorMode;
}

SGD_Bool SGD_DECL sgd_MouseButtonDown(int button) {
	return sgdx::mainWindow()->mouse()->button(button).down();
}

SGD_Bool SGD_DECL sgd_MouseButtonHit(int button) {
	return sgdx::mainWindow()->mouse()->button(button).hit();
}

SGD_Bool SGD_DECL sgd_GamepadConnected(int gamepad) {
	sgdx::mainWindow();

	if ((uint32_t)gamepad >= sgdx::Window::maxGamepads) sgd_Error("Gamepad index out of range");

	return sgdx::Gamepad::getGamepad(gamepad)->connected();
}

SGD_Bool SGD_DECL sgd_GamepadButtonDown(int gamepad, int button) {
	sgdx::mainWindow();

	if ((uint32_t)gamepad >= sgdx::Window::maxGamepads) sgd_Error("Gamepad index out of range");
	if ((uint32_t)button >= sgdx::Gamepad::numButtons) sgd_Error("Gamepad button out of range");

	return sgdx::Gamepad::getGamepad(gamepad)->button(button).down();
}

SGD_Bool SGD_DECL sgd_GamepadButtonHit(int gamepad, int button) {
	sgdx::mainWindow();

	if ((uint32_t)gamepad >= sgdx::Window::maxGamepads) sgd_Error("Gamepad index out of range");
	if ((uint32_t)button >= sgdx::Gamepad::numButtons) sgd_Error("Gamepad button out of range");

	return sgdx::Gamepad::getGamepad(gamepad)->button(button).hit();
}

float SGD_DECL sgd_GamepadAxis(int gamepad, int axis) {
	sgdx::mainWindow();

	if ((uint32_t)gamepad >= sgdx::Window::maxGamepads) sgd_Error("Gamepad index out of range");
	if ((uint32_t)axis >= sgdx::Gamepad::numAxes) sgd_Error("Gamepad index out of range");

	return sgdx::Gamepad::getGamepad(gamepad)->axis(axis);
}
