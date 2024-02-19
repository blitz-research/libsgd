#include <window/exports.h>

int main() {

	sgd::log() << "### Hello World!";

	auto window = new sgd::Window({640, 480}, "libSGD App", {});

	window->sizeChanged.connect(nullptr, [](sgd::CVec2u size) { sgd::log() << "### Size changed:" << size; });

	window->cursorPosChanged.connect(nullptr, [](sgd::CVec2f pos) {
		// sgd::log() << "### CursosPos changed:"<< pos;
	});

	window->closeClicked.connect(nullptr, [] { //
		std::exit(0);
	});


	for(;;){
		window->pollEvents();

		window->render();
	}
}
