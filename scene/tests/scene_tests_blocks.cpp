#include "start.cpp"

int numBlocks=10000;


void entry() {

	MeshPtr mesh = loadStaticMesh(Path("~/Desktop/terrain_v3/palm_tree1.glb")).result();
	fit(mesh, {-1, 1}, true);

	ModelPtr model = new Model();
	scene->add(model);
	model->mesh = mesh;

	createPlayer(nullptr);
	move(player, {0, 0, -2});

	for (;;) {
		pollEvents();

		playerFly(.025f);

		render();
	}
}

int main() {
	start(entry);
}
