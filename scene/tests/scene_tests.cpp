#include <scene/exports.h>

#include "start.cpp"

void entry() {

	for(;;) {
		pollEvents();

		render();
	}
}

int main() {
	start(entry);
}
