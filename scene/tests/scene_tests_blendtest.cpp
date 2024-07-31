#include "start.cpp"

void entry() {

	setConfigVar("debug.gltfLoader", "1");

	MeshPtr mesh = loadStaticMesh(Path("sgd://models/blendtest.glb")).result();
	fit(mesh, {-.5, .5}, true);

	int cells = 5;
	float spc = 2;
	float halfsz = ((float)cells * spc) / 2;

	Vector<Model*> models;

	for (int x = 0; x < cells; ++x) {
		for (int y = 0; y < cells; ++y) {
			for (int z = 0; z < cells; ++z) {
				auto model = new Model(mesh);
				scene->add(model);
				move(model, {(float)x * spc - halfsz, (float)y * spc - halfsz, (float)z * spc - halfsz});
				models.push_back(model);
			}
		}
	}

	createPlayer(nullptr);
	move(player, {0, 0, -halfsz});

	auto dc = overlay->drawList();

	for (;;) {
		pollEvents();

		seedRnd();
		for (auto model : models) {
			turn(model, {rnd(.1f), rnd(.2f), 0});
		}

		playerFly(.125f);

		dc->clear();
		dc->addText("RPS:" + toString(currentGC()->FPS()), {0, 0});
		dc->addText("RPS:" + toString(scene->sceneRenderer()->RPS()), {0, 16});

		render();
	}
}

int main() {
//	setConfigVar("dawn.validationEnabled", "0");
	start(entry);
}
