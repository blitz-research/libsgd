#include "start.cpp"

void entry() {


	{
		float sz = 1000;
//		MaterialPtr material = loadPBRMaterial(Path("sgd://misc/brownish-grass.jpg")).result();
		MaterialPtr material = loadPBRMaterial(Path("sgd://materials/Ground037_1K-JPG")).result();
		material->setTexture("roughnessTexture"	,whiteTexture());
		material->setFloat("roughnessFactor1f", .75f);
		MeshPtr mesh = createBoxMesh({{-sz/5, -1, -sz/5}, {sz/5, 0, sz/5}}, material);
		transformTexCoords(mesh, {sz, sz}, {0, 0});
		ModelPtr model = new Model(mesh);
		scene->add(model);
	}

	{
		float sz = 330;
		MeshPtr mesh = loadStaticMesh(Path("sgd://models/eiffel_tower.glb")).result();
		fit(mesh, {{-sz/2, 0, -sz/2}, {sz/2, sz, sz/2}}, true);
		mesh->shadowsEnabled = true;
		ModelPtr model = new Model(mesh);
		scene->add(model);
	}

	createPlayer(nullptr);
	move(player, {0, 1, -2});

	light->setWorldPosition({0,0,0});
	light->shadowsEnabled = true;
	setRotation(light, {-30,0,0});

	camera->far = 1024;
	scene->sceneBindings()->csmSplitDistances={16,64,256,1024};
	scene->sceneBindings()->csmTextureSize = 2048;
	scene->sceneBindings()->csmDepthBias=.0001f;

	auto overlay = new Overlay();
	scene->add(overlay);

	auto dc = overlay->drawList();

	for (;;) {
		pollEvents();

		playerFly(1);

		light->setWorldPosition({});

		rotate(light,{0,.025f,0});

		dc->clear();
		dc->addText("RPS:"+std::to_string(scene->RPS()), {0,0});

		render();
	}
}

int main() {
	start(entry);
}
