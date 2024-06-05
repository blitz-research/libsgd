#include "start.cpp"

void entry() {

	MeshPtr mesh = loadStaticMesh(Path("sgd://models/palm_tree1.glb")).result();
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
