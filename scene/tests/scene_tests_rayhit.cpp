#include "start.cpp"

void entry() {

	CameraPtr camera = new Camera(CameraType::perspective);
	scene->add(camera);

	MaterialPtr material = createPBRMaterial();
	material->setVector4f("albedoColor4f", {1, .25f, 0, 1});

	MeshPtr mesh = createTorusMesh(1, .25f, 96, 48, material);
	ModelPtr model = new Model();
	scene->add(model);

	auto data = new MeshColliderData(mesh);
	MeshColliderPtr collider = new MeshCollider(model, 0, 0, data);

	model->mesh = mesh;
	move(model, {0, 0, 5});

	OverlayPtr overlay = new Overlay();
	scene->add(overlay);

	overlay->drawList()->font = sgd::loadFont(sgd::Path("C:/windows/fonts/consola.ttf"), 12).result();

	for (;;) {

		turn(model, {1, .5, 0});

		overlay->drawList()->clear();
		{
			// Mouselook hack
			auto mouse = window->mouse()->position() / Vec2f(scene->viewportSize()) * 2.0f - 1.0f;
			camera->setLocalBasis(Mat3r::rotation({-mouse.y * halfPi, -mouse.x * pi, 0}));

			auto invProj = camera->inverseProjectionMatrix();
			auto tvec = Mat4r(invProj) * Vec4r(Vec3f(mouse.x, -mouse.y, 0), 1);
			auto dir = normalize(camera->worldMatrix().r * (Vec3r(tvec) / tvec.w));

			auto ray = Liner(camera->worldPosition(), dir);

			overlay->drawList()->addText("Ray:" + toString(ray), {0, 20});

			Contact contact(camera->far());

			auto hit = scene->collisionSpace()->intersectRay(ray, 0, ~0, contact);

			if (hit) overlay->drawList()->addText("HIT:" + toString(contact.time), {0, 40});
		}

		pollEvents();
		render();
	}
}

int main() {
	start(entry);
}
