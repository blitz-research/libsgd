#include "start.cpp"

#if 0
float anglediff(float src, float dst) {
}

float clampAngle(float angle, float min, float maxy) {

	float dmin =

}
#endif

void entry() {

	setConfigVar("gltf.loggingEnabled", "1");

	float size = 100.0f;

	MaterialPtr groundMaterial = loadPBRMaterial(Path("sgd://misc/grass1K.jpg")).result();
	MeshPtr groundMesh = createBoxMesh({{-size, 0, -size}, {size, 0, size}}, groundMaterial);
	transformTexCoords(groundMesh, {size / 2, size / 2}, {0, 0});

	ModelPtr groundModel = new Model(groundMesh);

	scene->add(groundModel);

	ModelPtr cannonModel = loadBonedModel(Path("~/Desktop/cannon.glb")).result();
	scene->add(cannonModel);

	Entity* turret = cannonModel->findChild("Turret");

	createPlayer(nullptr);
	move(player, {0, 10, -20});

	auto dc = overlay->drawList();

	BloomEffectPtr effect = new BloomEffect();
	effect->radius = 7;
	scene->sceneRenderer()->add(effect);

	for (;;) {
		pollEvents();

		//		aim(turret, player->worldPosition(), 0);
		//		auto yaw= sgd::yaw(turret->worldBasis());
		//		float d=clampAngle(yaw,-90,90);

		playerFly(.125f);

		dc->clear();
		dc->addText("FPS:" + std::to_string(currentGC()->FPS()), {0, 0});
		dc->addText("RPS:" + std::to_string(scene->sceneRenderer()->RPS()), {0, 16});

		render();
	}
}

int main() {
	start(entry);
}
