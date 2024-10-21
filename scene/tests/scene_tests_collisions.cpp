#include "start.cpp"

const int COLLISION_TYPE_LEVEL = 0;
const int COLLISION_TYPE_SPHERE = 1;
const int COLLISION_TYPE_PLAYER = 2;

void entry() {

	float sz = 50;

//	setConfigVar("gltf.loggingEnabled", "1");

	scene->collisionSpace()->enableCollisions(COLLISION_TYPE_PLAYER, COLLISION_TYPE_LEVEL, CollisionResponse::slidexz);
	scene->collisionSpace()->enableCollisions(COLLISION_TYPE_PLAYER, COLLISION_TYPE_SPHERE, CollisionResponse::slide);

	MeshPtr levelMesh = loadStaticMesh(Path("sgd://models/ManurewaDuplex.glb")).result();
	fit(levelMesh, Boxf(-sz, sz), true);

	ModelPtr levelModel = new Model();
	levelModel->mesh = levelMesh;
	scene->add(levelModel);
	ColliderPtr levelCollider = new MeshCollider(levelModel, COLLISION_TYPE_LEVEL, new MeshColliderData(levelMesh));

	createPlayer(nullptr);
	move(camera, {0, .8f, 0});
	setPosition(player, {-8, -4, 36});
	setRotation(player, {0, -145.5, 0});

	ColliderPtr playerCollider = new EllipsoidCollider(player, COLLISION_TYPE_PLAYER, .2f, 1.8f);

	MeshPtr sphereMesh = createSphereMesh(2.5f, 48, 24, createPBRMaterial());
	ModelPtr sphere = new Model(sphereMesh);
	scene->add(sphere);

	move(sphere,{0,-6,36});

	ColliderPtr sphereCollider = new MeshCollider(sphere, COLLISION_TYPE_SPHERE, new MeshColliderData(sphereMesh));

	auto dc = overlay->drawList();

	real gravity = -9.81 / 60.0 / 60.0;
	real vel = 0;
	bool jumping=false;

	for (;;) {
		pollEvents();

		float sp = 3.2f;
		if (window->keyboard()->key(KeyCode::LEFT_SHIFT).down()) sp = 5.6f;

		playerWalk(sp / 60.0f);

		if (window->keyboard()->key(KeyCode::SPACE).hit()) {
			jumping=true;
			vel += .1f;
		}

		real py = player->worldPosition().y;

		vel += gravity;

		move(player, {0, vel, 0});

		scene->collisionSpace()->updateColliders();

		vel = player->worldPosition().y - py;

		if(jumping && vel<0.0f) jumping = false;

		if(!jumping && vel>0.0f) vel=0.0f;

		dc->clear();
		auto& colls = playerCollider->collisions();
		for(int i=0;i<colls.size();++i) {
			dc->addText("Collision "+toString(i)+": colliderType "+ toString(colls[i].collider->colliderType()), Vec2u(0, i * 16));
		}

		render();
	}
}

int main() {
	start(entry);
}
