#include "start.cpp"

void entry() {

	MeshPtr mesh = loadStaticMesh(Path("~/dev/assets/Wall_Modular.glb")).result();
	fit(mesh, {-1, 1}, true);
	ModelPtr model = new Model();
	model->mesh = mesh;
	scene->add(model);
	move(model, {0,0,15});

	log() << "###"<<model->worldMatrix();

#if 0
	ModelPtr model = loadSkinnedModel(Path("~/Desktop/larry_walk.glb")).result();
	move(model, {0, 0, 5});
	scene->add(model);
#endif

	for (;;) {

		pollEvents();
		turn(model,{0,1,0});
#if 0
		static float time;
		model->animate(0, time += 1.0/60.0, AnimationMode::loop);
#endif
		render();
	}
}

int main() {
	start(entry);
}
