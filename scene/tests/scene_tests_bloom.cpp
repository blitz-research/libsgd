#include "start.cpp"

void entry() {

	skybox->setIsVisible(false);

	BloomEffectPtr bloom = new BloomEffect();
	scene->sceneRenderer()->add(bloom);

	float size=100.0f;

	MeshPtr crateMesh = loadStaticMesh(Path("~/Desktop/evening_begins_-_3d_editor_challenge.glb")).result();
	fit(crateMesh, {{-size / 2, -size / 2, -size / 2}, {size / 2, size/2, size / 2}}, true);

	ModelPtr model = new Model(crateMesh);
	scene->add(model);

	createPlayer(nullptr);
	move(player,{0,0,-3});

	auto dc = overlay->drawList();

	for(;;) {
		pollEvents();

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
