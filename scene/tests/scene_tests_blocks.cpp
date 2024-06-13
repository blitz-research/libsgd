#include "start.cpp"

int numBlocks = 10000;

void entry() {

	setRotation(light, {-75, 25, 0});
	light->castsShadow = true;

	{
		float sz = 32;
		MaterialPtr material = loadPBRMaterial(Path("sgd://misc/brownish-grass.jpg")).result();
		MeshPtr mesh = createBoxMesh({{-sz, -1, -sz}, {sz, 0, sz}}, material);
		transformTexCoords(mesh, {sz, sz}, {0, 0});
		ModelPtr model = new Model(mesh);
		scene->add(model);
	}

	{
		MeshPtr mesh = loadStaticMesh(Path("sgd://models/sci-fi_crate.glb")).result();
		mesh->castsShadow=true;
		fit(mesh, {-.4f, .4f}, true);
		for (int x = -20; x <= 20; ++x) {
			for (int z = -20; z <= 20; ++z) {
				ModelPtr model = new Model();
				scene->add(model);
				model->mesh = mesh;
				move(model, {(float)x*2, (float)(20-std::sqrt(x*x+z*z)), (float)z*2});
			}
		}
	}

	createPlayer(nullptr);
//	move(player, {0, 2, -4});
	move(player, {0,10,0});
	turn(player, {-90,0,0});

	camera->near = .1f;
	camera->far = 256;
	scene->sceneBindings()->csmSplits ={16,64,192,256};

	for (;;) {
		pollEvents();

		playerFly(.125f);

//		SGD_LOG << camera->worldPosition();

		render();
	}
}

int main() {
	start(entry);
}
