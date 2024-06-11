#include "start.cpp"

void entry() {

	float sz=32;

	{
		MaterialPtr material = loadPBRMaterial(Path("sgd://misc/brownish-grass.jpg")).result();
		MeshPtr mesh = createBoxMesh({{-sz, -1, -sz}, {sz, 0, sz}}, material);
		transformTexCoords(mesh, {sz, sz}, {0, 0});
		ModelPtr model = new Model(mesh);
		scene->add(model);
	}

	MeshPtr meshes[3];
	meshes[0] = loadStaticMesh(Path("sgd://models/tree1.glb")).result();
	meshes[1] = loadStaticMesh(Path("sgd://models/palm_tree1.glb")).result();
	meshes[2] = loadStaticMesh(Path("sgd://models/birch_tree1.glb")).result();

	for(int i=0;i<50;++i) {
		ModelPtr model = new Model();
		scene->add(model);
		move(model, {rnd(-sz,sz),0,rnd(-sz,sz)});
		model->mesh = meshes[(int)rnd(3)];
	}

	createPlayer(nullptr);
	move(player, {0, 1, -2});

	for (;;) {
		pollEvents();

		playerFly(.25f);

		render();
	}
}

int main() {
	start(entry);
}
