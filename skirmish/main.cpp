#include <sgd/sgd.h>

#include <string>

#include "game.h"

#include "actor.h"

int main() {

	sgd_Init();

	sgd_CreateWindow(sgd_GetDesktopWidth() / 2, sgd_GetDesktopHeight() / 2, "Skirmish!", SGD_WINDOW_FLAGS_RESIZABLE);

	//sgd_SetClearColor(1,.5,0,1);

	//auto font = sgd_LoadFont("sgd://skirmish/BalooBhai2-Bold.ttf", 24);
	//sgd_Set2DFont(font);

	skirmish::initActorTypes();

	skirmish::game = new skirmish::Game();

	skirmish::game->begin();

	while (!(sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED)) {

		skirmish::game->update();

		sgd_Clear2D();

		sgd_Draw2DText(("FPS:" + std::to_string(sgd_GetFPS())).c_str(), 0, 0);
		sgd_Draw2DText(("FPS:" + std::to_string(sgd_GetRPS())).c_str(), 0, 16);

		sgd_RenderScene();

		sgd_Present();
	}

	sgd_Terminate();
}
