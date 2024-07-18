#include "start.cpp"

void entry() {

	ModelPtr model = loadSkinnedModel(Path("sgd://models/larry_walk.glb")).result();
	move(model, {0, 0, 5});
	scene->add(model);

	for (;;) {

		pollEvents();
		turn(model, {0, 1, 0});

		static float time;
		model->animate(0, time += 1.0 / 60.0, AnimationMode::loop, 1);
		render();
	}
}

int main() {
	start(entry);
}
