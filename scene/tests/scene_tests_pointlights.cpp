#include "start.cpp"

void entry() {

	scene->ambientLightColor = {0, 0, 0, 1};

	// Height of eifeel tower!
	float size = 330.0f;

	MaterialPtr groundMaterial = loadPBRMaterial(Path("sgd://materials/Ground037_1K-JPG")).result();
	groundMaterial->setTexture("roughnessTexture", whiteTexture());
	groundMaterial->setFloat("roughnessFactor1f", .75f);
	MeshPtr groundMesh = createBoxMesh({{-size, 0, -size}, {size, 0, size}}, groundMaterial);
	transformTexCoords(groundMesh, {size, size}, {0, 0});

	MeshPtr towerMesh = loadStaticMesh(Path("sgd://models/eiffel_tower.glb")).result();
	fit(towerMesh, {{-size / 2, 0, -size / 2}, {size / 2, size, size / 2}}, true);
	towerMesh->shadowsEnabled = true;

	for (int ix = 0; ix <= 0; ++ix) {
		for (int iz = 0; iz <= 0; ++iz) {
			ModelPtr groundModel = new Model(groundMesh);
			ModelPtr towerModel = new Model(towerMesh);
			towerModel->setParent(groundModel);
			move(groundModel, {(float)ix * size * 2, 0, (float)iz * size * 2});
			scene->add(groundModel);
		}
	}

	//	light->setIsVisible(false);

	LightPtr lights[3];

	ModelPtr pivot = new Model();
	scene->add(pivot);

	for (int i = 0; i < 3; ++i) {
		lights[i] = new Light(LightType::point);
		scene->add(lights[i]);
		lights[i]->shadowsEnabled = true;
		lights[i]->setParent(pivot);
		lights[i]->color = Vec4f(i==0, i==1, i==2, 1);
		lights[i]->range = 180;
		turn(lights[i], {0, 120.0f * i,0});
		move(lights[i], {0, 30, 60});
		break;
	}

	scene->sceneBindings()->psmClipNear=1;

	createPlayer(nullptr);
	move(player, {0, 20, -100});

	auto overlay = new Overlay();
	scene->add(overlay);

	auto dc = overlay->drawList();

	for (;;) {
		pollEvents();

		playerFly(1.25f);

		rotate(pivot, {0, .25f, 0});

		dc->clear();
		dc->addText("FPS:" + std::to_string(gc->FPS()), {0, 0});
		dc->addText("RPS:" + std::to_string(scene->RPS()), {0, 16});

		render();
	}
}

int main() {
	start(entry);
}
