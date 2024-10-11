#include "start.cpp"

void entry() {

	setConfigVar("debug.gltfLoader","1");

	float sz = 10;

	{
		MaterialPtr material = loadPBRMaterial(Path("sgd://misc/brownish-grass.jpg")).result();
		MeshPtr mesh = createBoxMesh({{-sz, -1, -sz}, {sz, 0, sz}}, material);
		transformTexCoords(mesh, {sz, sz}, {0, 0});
		ModelPtr model = new Model(mesh);
//		scene->add(model);
	}

	MeshPtr meshes[3];
	meshes[0] = loadStaticMesh(Path("sgd://models/tree1.glb")).result();
	meshes[1] = loadStaticMesh(Path("sgd://models/palm_tree1.glb")).result();
	meshes[2] = loadStaticMesh(Path("sgd://models/birch_tree1.glb")).result();

	int n = 5;
	for (int i = 0; i < n; ++i) {
		ModelPtr model = new Model(meshes[(int)rnd(3)]);
		scene->add(model);
		move(model, {rnd(-sz, sz), 0, rnd(-sz, sz)});
	}

	auto json = serialize(scene);
	SGD_LOG << json;

	scene->clear();

	scene = deserialize(json)->as<Scene>();

//	std::exit(0);

	createPlayer(nullptr);
	move(player, {0, 1, -2});

	SharedPtr dc  =overlay->drawList();

	for (;;) {
		pollEvents();

		playerFly(.125f);

		dc->clear();
		dc->addText(String("FPS:")+toString(currentGC()->FPS()),{0,0});

		render();
	}
}

int main() {
	start(entry);
}
