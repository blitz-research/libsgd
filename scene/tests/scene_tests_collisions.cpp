#include "start.cpp"

Vec3r collide(Vec3r src, Vec3r dst, real radius, uint32_t collisionMask) {
	if (dst == src) return src;

	static constexpr real eps = (real).0001;

	Planer hitPlanes[3];
	int n = 0;
	int i = 0;

	for (;;) {

		auto dir = dst - src;

		auto d = length(dir);
		if (d <= eps) return src;

		Contact contact(d);

		Liner ray(src, dir / d);

		if (!scene->collisionSpace()->intersectRay(ray, radius, collisionMask, contact)) {
			return dst;
		}

		SGD_ASSERT(contact.time >= 0);
		++n;

		hitPlanes[i] = {contact.point, contact.normal};
		hitPlanes[i].d -= eps;

		src = ray * std::max(contact.time - eps, (real)0.0);

		dst = nearest(hitPlanes[i], dst);

		switch (i) {
		case 0: {
			// First hit, move along plane
			i = 1;
			break;
		}
		case 1: {
			// Second hit, move along plane?
			if (distance(hitPlanes[0], dst) >= 0) {
				i = 0;
				break;
			}
			// Nope, move along crease
			auto crease = normalize(cross(hitPlanes[0].n, hitPlanes[1].n));
			dst = src + crease * dot(dst - src, crease);
			i = 2;
			break;
		}
		case 2: {
			// Third hit, move along plane?
			if (distance(hitPlanes[0], dst) >= 0 && distance(hitPlanes[1], dst) >= 0) {
				i = 0;
				break;
			}
			// Nope, we're stuck in intersection of 3 planes.
			return dst;
		}
		default:
			SGD_ABORT();
		}
	}
}

void entry() {

	CameraPtr camera = new Camera(CameraType::perspective);
	move(camera, {0, 2.5f, -10});
	scene->add(camera);

	MaterialPtr material = createPBRMaterial();
	material->setVector4f("albedoColor4f", {.2f, .7f, 0, 1});
	ModelPtr ground = new Model();
	ground->mesh = createBoxMesh({{-10, -1, -10}, {10, 0, 10}}, material);
	scene->add(ground);
	turn(ground,{0, 0, 3});

	auto groundColliderData = new MeshColliderData(ground->mesh());
	ColliderPtr groundCollider = new MeshCollider(ground, 0, 0, groundColliderData);

	material = createPBRMaterial();
	material->setVector4f("albedoColor4f", {1, .7f, 0, 1});
	ModelPtr sphere = new Model();
	sphere->mesh = createSphereMesh(1, 96, 48, material);
	move(sphere, {0, 5, 0});
	scene->add(sphere);

	Vec3r gravity = {0, -9.81 / 60.0 / 60.0, 0};
	Vec3r vel{};

	for (;;) {
		pollEvents();

		auto src = sphere->worldPosition();
		vel += gravity;
		auto dst = src + vel;
		dst = collide(src, dst, 1, ~0);
		sphere->setWorldPosition(dst);
		vel = dst - src;

		render();
	}
}

int main() {
	start(entry);
}
