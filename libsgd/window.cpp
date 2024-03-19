#include "internal.h"

namespace {

sgd::Vec2u g_windowSize;

int g_windowEvents;

} // namespace

void SGD_DECL sgd_CreateWindow(int width, int height, SGD_String title, int flags) {
	if(sgd::mainWindow) sgd_Error("Window has already been created");

	sgd::mainWindow = new sgd::Window(sgd::Vec2u(width, height), title, (sgd::WindowFlags)flags);

	sgd::mainWindow->sizeChanged.connect(nullptr, [](sgd::CVec2u size) {
		g_windowSize = size;
		g_windowEvents |= SGD_EVENT_MASK_SIZE_CHANGED;
	});

	sgd::mainWindow->closeClicked.connect(nullptr, [] { //
		g_windowEvents |= SGD_EVENT_MASK_CLOSE_CLICKED;
	});

	g_windowSize = sgd::mainWindow->size();
}

int SGD_DECL sgd_WindowWidth() {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");

	return(int)g_windowSize.x;
}

int SGD_DECL sgd_WindowHeight() {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");

	return (int)g_windowSize.y;
}

int SGD_DECL sgd_PollEvents() {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");

	g_windowEvents = 0;
	sgd::mainWindow->pollEvents();

	return g_windowEvents;
}

SGD_Bool SGD_DECL sgd_KeyDown(int keyCode) {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");
	if((uint32_t)keyCode > SGD_KEY_LAST) sgd_Error("Key code out of range");

	return sgd::mainWindow->keyboard()->key(keyCode).down();
}

SGD_Bool SGD_DECL sgd_KeyHit(int keyCode) {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");
	if((uint32_t)keyCode > SGD_KEY_LAST) sgd_Error("Key code out of range");

	return sgd::mainWindow->keyboard()->key(keyCode).hit();
}

int SGD_DECL sgd_GetChar() {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");

	auto& queue = sgd::mainWindow->keyboard()->charQueue();
	if(queue.empty()) return 0;

	auto chr = queue.front();
	queue.pop_front();
	return chr;
}

float SGD_DECL sgd_MouseX() {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");

	return sgd::mainWindow->mouse()->position().x;
}

float SGD_DECL sgd_MouseY() {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");

	return sgd::mainWindow->mouse()->position().y;
}

float SGD_DECL sgd_MouseScrollX() {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");

	return sgd::mainWindow->mouse()->scroll().x;
}

float SGD_DECL sgd_MouseScrollY() {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");

	return sgd::mainWindow->mouse()->scroll().y;
}

SGD_Bool SGD_DECL sgd_MouseButtonDown(int button) {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");

	return sgd::mainWindow->mouse()->button(button).down();
}

SGD_Bool SGD_DECL sgd_MouseButtonHit(int button) {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");

	return sgd::mainWindow->mouse()->button(button).hit();
}

SGD_Bool SGD_DECL sgd_GamepadConnected(int gamepad) {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");
	if((uint32_t)gamepad >= sgd::Window::maxGamepads) sgd_Error("Gamepad index out of range");

	return sgd::Gamepad::getGamepad(gamepad)->connected();
}

SGD_Bool SGD_DECL sgd_GamepadButtonDown(int gamepad, int button) {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");
	if((uint32_t)gamepad >= sgd::Window::maxGamepads) sgd_Error("Gamepad index out of range");
	if((uint32_t)button >= sgd::Gamepad::numButtons) sgd_Error("Gamepad button out of range");

	return sgd::Gamepad::getGamepad(gamepad)->button(button).down();
}

SGD_Bool SGD_DECL sgd_GamepadButtonHit(int gamepad, int button) {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");
	if((uint32_t)gamepad >= sgd::Window::maxGamepads) sgd_Error("Gamepad index out of range");
	if((uint32_t)button >= sgd::Gamepad::numButtons) sgd_Error("Gamepad button out of range");

	return sgd::Gamepad::getGamepad(gamepad)->button(button).hit();
}

float SGD_DECL sgd_GamepadAxis(int gamepad, int axis) {
	if(!sgd::mainWindow) sgd_Error("Window has not been created");
	if((uint32_t)gamepad >= sgd::Window::maxGamepads) sgd_Error("Gamepad index out of range");
	if((uint32_t)axis >= sgd::Gamepad::numAxes) sgd_Error("Gamepad index out of range");

	return sgd::Gamepad::getGamepad(gamepad)->axis(axis);
}
