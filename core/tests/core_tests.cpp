#include "core/exports.h"

using namespace sgd;

int main() {
	log() << "### Hello world!";
	log() << "### homeDir:" << Path("~").filePath();
}
