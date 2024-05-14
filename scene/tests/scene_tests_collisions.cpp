#include "start.cpp"

void entry() {

	//	CameraPtr camera = new Camera(CameraType::perspective);
	//	move(camera, {0, 2.5f, -60});
	//	scene->add(camera);

	scene->collisionSpace()->enableCollisions(1, 0, CollisionResponse::slide);

	MeshPtr levelMesh = loadStaticMesh(Path("sgd://models/ManurewaDuplex.glb")).result();
	fit(levelMesh, Boxf({-50, -50, -50}, {50, 50, 50}), true);

	ModelPtr levelModel = new Model();
	levelModel->mesh = levelMesh;
	scene->add(levelModel);
	ColliderPtr levelCollider = new MeshCollider(levelModel, 0, new MeshColliderData(levelMesh));

	MeshPtr playerMesh = createSphereMesh(1, 64, 32, nullptr);

	ModelPtr playerModel = new Model();
	playerModel->mesh = playerMesh;
	scene->add(playerModel);
	move(playerModel, {0, 2.5f, -30});
	ColliderPtr playerCollider = new SphereCollider(playerModel, 1, 1);
	CameraPtr playerCamera = new Camera(CameraType::perspective);
	scene->add(playerCamera);
	playerCamera->setParent(playerModel);

	scene->collisionSpace()->enableCollisions(1, 0, CollisionResponse::slide);

	auto overlay = new Overlay();
	scene->add(overlay);

	auto dc = overlay->drawList();
	dc->font = loadFont(Path("C:/windows/fonts/consola.ttf"), 20).result();

	real gravity = -9.81 / 60.0 / 60.0;
	real vel = 0;

	for (;;) {
		pollEvents();

		if (window->keyboard()->key(KeyCode::LEFT).down()) {
			turn(playerModel, {0, 1, 0});
		} else if (window->keyboard()->key(KeyCode::RIGHT).down()) {
			turn(playerModel, {0, -1, 0});
		}
		if (window->keyboard()->key(KeyCode::W).down()) {
			move(playerModel, {0, 0, .1f});
		} else if (window->keyboard()->key(KeyCode::S).down()) {
			move(playerModel, {0, 0, -.1f});
		}

		auto src = playerModel->worldPosition().y;
		vel += gravity;
		move(playerModel, {0, vel, 0});

		scene->collisionSpace()->updateColliders();

		real newVel = std::min(playerModel->worldPosition().y - src, (real)0);

		vel = newVel;

		dc->clear();
		dc->addText(toString(vel), {0, 0});

		render();
	}
}

int main() {
	start(entry);
}
