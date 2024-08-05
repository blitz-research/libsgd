#include "start.cpp"

void entry() {

	float sz = 50;

	MeshPtr levelMesh = loadStaticMesh(Path("sgd://models/ManurewaDuplex.glb")).result();
	fit(levelMesh, Boxf(-sz, sz), true);

	ModelPtr levelModel = new Model();
	levelModel->mesh = levelMesh;
	scene->add(levelModel);
	ColliderPtr levelCollider = new MeshCollider(levelModel, 0, new MeshColliderData(levelMesh));

	createPlayer(nullptr);
	move(camera, {0, .8f, 0});
	setPosition(player, {-8, -4, 36});
	setRotation(player, {0, -145.5, 0});

	ColliderPtr playerCollider = new EllipsoidCollider(player, 1, .2f, 1.8f);

	scene->collisionSpace()->enableCollisions(1, 0, CollisionResponse::slidexz);

	auto dc = overlay->drawList();

	real gravity = -9.81 / 60.0 / 60.0;
	real vel = 0;

	for (;;) {
		pollEvents();

		float sp = 3.2f;
		if (window->keyboard()->key(KeyCode::LEFT_SHIFT).down()) sp = 5.6f;

		playerWalk(sp / 60.0f);

		if (window->keyboard()->key(KeyCode::SPACE).hit()) {
			vel += .1f;
		}

		real py = player->worldPosition().y;

		vel += gravity;

		move(player, {0, vel, 0});

		scene->collisionSpace()->updateColliders();

		vel = player->worldPosition().y - py;

		dc->clear();
		dc->addText(toString(vel)+","+toString(gravity),{0,0});
//		dc->addText(toString(camera->worldPosition()) + ", " + toString(camera->worldPosition()), {0, 0});
//		dc->addText(toString(scene->sceneRenderer()->RPS()), {0, 20});

		render();
	}
}

int main() {
	start(entry);
}
