#include "start.cpp"

void entry() {

	MeshPtr mesh = loadStaticMesh(Path("~/dev/assets/palm_tree1.glb")).result();
	fit(mesh, {-1, 1}, true);

	ModelPtr model = new Model();
	scene->add(model);
	model->mesh = mesh;

	CameraPtr camera = new Camera(CameraType::perspective);
	scene->add(camera);

	move(camera, {0, 3, 0});

	turn(camera, {-90, 0, 0});

	for (;;) {
		pollEvents();

//		turn(model, {0, 1, 0});

		fly(camera);

		render();
	}
}

int main() {
	start(entry);
}
