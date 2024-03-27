#include "core/exports.h"

using namespace sgd;

int main() {
	log() << "### Hello world!";
	log() << "### appDir:" << Path("${APP}").filePath();
	log() << "### homeDir:" << Path("${HOME}").filePath();
}
