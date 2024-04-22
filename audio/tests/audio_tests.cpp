
#include "audio/exports.h"

int main(){

	sgd::initApp();

	auto sound = sgd::loadSound(sgd::Path("sgd://audio/fine_morning.ogg")).result();

	sgd::log() << "### Hello";

	sgd::playSound(sound);

	sgd::log() << "### Hello again";

	sgd::beginAppEventLoop();
}
