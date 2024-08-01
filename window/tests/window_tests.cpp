#include <window/exports.h>

using namespace sgd;

int main() {

	initApp();

	auto window = new Window({640, 480}, "libSGD App", WindowFlags::resizable);

	static bool logPos = true;

	window->sizeChanged.connect(nullptr, [](CVec2u size) { //
		SGD_LOG << "Size changed:" << size;
		logPos = true;
	});

	window->positionChanged.connect(nullptr, [](CVec2i position) { //
		if (logPos) SGD_LOG << "Position changed:" << position;
		logPos = false;
	});

	window->stateChanged.connect(nullptr, [](WindowState state) { //
		SGD_LOG << "State changed:" << (int)state;
	});

	window->closeClicked.connect(nullptr, [] { //
		exitApp();
		std::exit(0);
	});

	for (;;) {
		pollEvents();
		if (window->keyboard()->key(KeyCode::F11).hit()) {
			if (window->state() != WindowState::fullscreen) {
				window->setState(WindowState::fullscreen);
			} else {
				window->setState(WindowState::normal);
			}
		}
		_sleep(16);
	}
}
