#include "start.cpp"

/*
void entry() {
	for (;;) {
		pollEvents();

		render();
	}
}

int main() {
	start(entry);
}
*/

void entry() {

	MeshPtr mesh = loadStaticMesh(Path("~/Desktop/test/ambulance.glb")).result();
	fit(mesh, {-1, 1}, true);

	ModelPtr model = new Model(mesh);
	scene->add(model);
	move(model, {0, 0, 5});

	for (;;) {
		pollEvents();
		render();
	}
}

int main() {
	start(entry);
}
