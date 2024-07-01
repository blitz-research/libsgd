
#include "audio/exports.h"

int main() {

	sgd::initApp();

	auto path  = "~/dev/assets/Back_In_Black.ogg";

	sgd::playMusic(sgd::Path(path));

	sgd::log() << "### Waiting...";

	sgd::debugObjects(false);

	sgd::beginAppEventLoop();
}
