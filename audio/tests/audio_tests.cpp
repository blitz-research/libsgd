
#include "audio/exports.h"

int main() {

	sgd::initApp();

//	auto path  = "sgd://audio/Back_In_Black.ogg";
	auto path  = "~/dev/assets/Back_In_Black.ogg";

	sgd::playMusic(sgd::Path(path));

	sgd::log() << "### Waiting...";

	sgd::beginAppEventLoop();
}
