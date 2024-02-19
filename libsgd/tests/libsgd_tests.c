#include <libsgd/libsgd.h>

#include <stdio.h>
#include <stdlib.h>

void sgd_Main() {
	puts("sgd_Main!");

	for(;;) {

		int mask = sgd_PollEvents();

		if(mask & SGD_WINDOW_EVENT_MASK_CLOSE_CLICKED) exit(0);

		if(mask & SGD_WINDOW_EVENT_MASK_SIZE_CHANGED) {
			printf("Window size changed: %i, %i\n", sgd_WindowWidth(), sgd_WindowHeight());
			fflush(stdout);
		}

		sgd_Render();
	}

	exit(0);
}

int main() {
	puts("Hello World!");

	sgd_CreateWindow(640, 480, "Hello LibSGD!", SGD_WINDOW_FLAGS_RESIZABLE);

	sgd_Run(&sgd_Main);
}
