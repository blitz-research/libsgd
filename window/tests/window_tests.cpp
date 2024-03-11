#include <window/exports.h>

int main() {

	sgd::log() << "### Hello World!";

	auto window = new sgd::Window({640, 480}, "libSGD App", {});

	window->sizeChanged.connect(nullptr, [](sgd::CVec2u size) { sgd::log() << "### Size changed:" << size; });

	window->closeClicked.connect(nullptr, [] { //
		std::exit(0);
	});

	while(window->pollEvents()) {
	}
}
