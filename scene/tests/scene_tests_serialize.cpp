#include "start.cpp"

void entry() {

	setConfigVar("debug.gltfLoader","1");

	float sz = 10;///0;

	int n_grasses=10;//00;
	int n_trees=5;//000;

	{
		MaterialPtr material = loadPBRMaterial(Path("sgd://misc/brownish-grass.jpg")).result();
		PlaneEntityPtr plane = new PlaneEntity();
		scene->add(plane);
		plane->bindings()->material = material;
	}

	{
		ImagePtr image = loadImage(Path("sgd://misc/grass1.png")).result();
		image->viewMode = ImageViewMode::upright;
		image->rect = Rectf(-.5f,0,.5f,.5f);

		for (int i = 0; i < n_grasses; ++i) {
			SpritePtr sprite = new Sprite(image);
			scene->add(sprite);
			move(sprite, {rnd(-sz/2,sz/2),0,rnd(-sz/2,sz/2)});
			auto sc=rnd(.5f,.75f);
			setScale(sprite,{sc,sc,sc});
		}
	}

	{
		MeshPtr meshes[3];
		meshes[0] = loadStaticMesh(Path("sgd://models/tree1.glb")).result();
		meshes[1] = loadStaticMesh(Path("sgd://models/palm_tree1.glb")).result();
		meshes[2] = loadStaticMesh(Path("sgd://models/birch_tree1.glb")).result();

		for (int i = 0; i < n_trees; ++i) {
			ModelPtr model = new Model(meshes[(int)rnd(3)]);
			scene->add(model);
			move(model, {rnd(-sz/2, sz/2), 0, rnd(-sz/2, sz/2)});
		}
	}

	auto json = serialize(scene);
	saveString( json, Path("~/Desktop/scene.sgd"));
//	SGD_LOG << json;

//#if 0
	scene->clear();
	scene = deserialize(json)->as<Scene>();
	overlay = new Overlay();
	scene->add(overlay);
//#endif

//	std::exit(0);

	createPlayer(nullptr);
	move(player, {0, 1, -2});

	SharedPtr dc =overlay->drawList();

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
