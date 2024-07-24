#include "meshprimitives.h"

#include "meshutil.h"

#include "../material/materialutil.h"

namespace sgd {

namespace {

Mesh* createMesh(CVector<Vertex> vertices, CVector<Triangle> triangles, Material* material) {

	if (!material) material = createPBRMaterial();
	MeshFlags flags = material->hasNormalTexture() ? MeshFlags::tangentsEnabled : MeshFlags::none;

	auto mesh = new Mesh(vertices.size(), flags);
	sgd::copy(mesh->lockVertices(), vertices.data(), vertices.size());
	mesh->unlockVertices();

	auto surface = new Surface(triangles.size(), material);
	sgd::copy(surface->lockTriangles(), triangles.data(), triangles.size());
	surface->unlockTriangles();

	mesh->addSurface(surface);

	if(bool(flags & MeshFlags::tangentsEnabled)) updateTangents(mesh);

	return mesh;
}

} // namespace

Mesh* createBoxMesh(CBoxf box, Material* material) {

	Vector<Vertex> vertices;
	Vector<Triangle> triangles;

	Vec3f verts[]{{box.min.x, box.max.y, box.min.z}, {box.max.x, box.max.y, box.min.z}, {box.max.x, box.min.y, box.min.z},
				  {box.min.x, box.min.y, box.min.z}, {box.min.x, box.max.y, box.max.z}, {box.max.x, box.max.y, box.max.z},
				  {box.max.x, box.min.y, box.max.z}, {box.min.x, box.min.y, box.max.z}};

	Vec3f norms[]{{0, 0, -1}, {1, 0, 0}, {0, 0, 1}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}};

	uint32_t faces[]{0, 1, 2, 3, 1, 5, 6, 2, 5, 4, 7, 6, 4, 0, 3, 7, 4, 5, 1, 0, 3, 2, 6, 7};

	for (int i = 0; i < 24; i += 4) {
		CVec3f norm = norms[i / 4];
		vertices.emplace_back(verts[faces[i + 0]], norm, Vec2f(0, 0));
		vertices.emplace_back(verts[faces[i + 1]], norm, Vec2f(1, 0));
		vertices.emplace_back(verts[faces[i + 2]], norm, Vec2f(1, 1));
		vertices.emplace_back(verts[faces[i + 3]], norm, Vec2f(0, 1));
		triangles.emplace_back(i, i + 2, i + 1);
		triangles.emplace_back(i, i + 3, i + 2);
	}

	return createMesh(vertices, triangles, material);
}

Mesh* createSphereMesh(float radius, uint32_t xSegs, uint32_t ySegs, Material* material) {

	Vector<Vertex> vertices;
	Vector<Triangle> triangles;

	auto fxSegs = 1.0f / (float)xSegs, fySegs = 1.0f / (float)ySegs;

	for (int i = 0; i < xSegs; ++i)
		vertices.emplace_back(Vec3f(0, radius, 0), Vec3f(0, 1, 0), Vec2f(((float)i + .5f) * 2 * fxSegs, 0));
	for (int j = 1; j < ySegs; ++j) {
		float pitch = halfPi - (float)j * pi * fySegs;
		for (int i = 0; i <= xSegs; ++i) {
			float yaw = (float)i * twoPi * fxSegs;
			float r = cosf(pitch);
			float y = sinf(pitch);
			float x = cosf(yaw) * r;
			float z = sinf(yaw) * r;
			vertices.emplace_back(Vec3f(x, y, z) * radius, Vec3f(x, y, z), Vec2f(float(i) * 2 * fxSegs, float(j) * fySegs));
		}
	}
	for (int i = 0; i < xSegs; ++i)
		vertices.emplace_back(Vec3f(0, -radius, 0), Vec3f(0, -1, 0), Vec2f(((float)i + .5f) * 2 * fxSegs, 1));

	for (int i = 0; i < xSegs; ++i) triangles.emplace_back(i, i + xSegs, i + xSegs + 1);
	for (int j = 1; j < ySegs - 1; ++j) {
		for (int i = 0; i < xSegs; ++i) {
			int v0 = j * (xSegs + 1) + i - 1;
			int v2 = v0 + xSegs + 2;
			triangles.emplace_back(v0, v2, v0 + 1);
			triangles.emplace_back(v0, v2 - 1, v2);
		}
	}
	for (int i = 0; i < xSegs; ++i) {
		auto v0 = (xSegs + 1) * (ySegs - 1) + i - 1;
		triangles.emplace_back(v0, v0 + xSegs + 1, v0 + 1);
	}

	return createMesh(vertices, triangles, material);
}

Mesh* createCylinderMesh(float height, float radius, uint32_t segs, Material* material) {

	Vector<Vertex> vertices;
	Vector<Triangle> triangles;

	float top = height / 2, bot = -top;

	// Sides
	for (int i = 0; i <= segs; ++i) {
		float yaw = i * twoPi / segs;
		auto v = Vec3f(std::cos(yaw) * radius, top, std::sin(yaw) * radius);
		auto n = normalize(Vec3f(v.x, 0, v.z));
		auto t = Vec2f(float(i) / segs, 0);
		vertices.emplace_back(v, n, t);
		v.y = bot;
		t.y = 1;
		vertices.emplace_back(v, n, t);
	}
	for (int i = 0; i < segs; ++i) {
		triangles.emplace_back(i * 2, i * 2 + 3, i * 2 + 2);
		triangles.emplace_back(i * 2, i * 2 + 1, i * 2 + 3);
	}

	// Caps
	int v0 = vertices.size();
	for (int i = 0; i < segs; ++i) {
		float yaw = i * twoPi / segs;
		auto v = Vec3f(std::cos(yaw) * radius, top, std::sin(yaw) * radius);
		auto n = Vec3f(0, 1, 0);
		auto t = Vec2f(v.x * .5f + .5f, 1 - (v.z * .5f + .5f));
		vertices.emplace_back(v, n, t);
		v.y = bot;
		n.y = -n.y;
		vertices.emplace_back(v, n, t);
	}
	for (int i = 1; i < segs - 1; ++i) {
		triangles.emplace_back(v0 + (i + 1) * 2, v0, v0 + i * 2);
		triangles.emplace_back(v0 + i * 2 + 1, v0 + 1, v0 + (i + 1) * 2 + 1);
	}

	return createMesh(vertices, triangles, material);
}

Mesh* createConeMesh(float length, float radius, uint32_t segs, Material* material) {

	Vector<Vertex> vertices;
	Vector<Triangle> triangles;

	float top = length / 2, bot = -top;

	// Peak
	for (int i = 0; i < segs; ++i) {
		vertices.emplace_back(Vec3f(0, top, 0), Vec3f(0, 1, 0), Vec2f((i + 0.f) / segs, 0));
	}

	// Sides
	for (int i = 0; i <= segs; ++i) {
		float yaw = i * twoPi / segs;
		auto n = Vec3f(std::cos(yaw), 0, std::sin(yaw));
		auto v = Vec3f(n.x * radius, bot, n.z * radius);
		auto t = Vec2f(float(i) / segs, 1);
		vertices.emplace_back(v, n, t);
	}
	for (int i = 0; i < segs; ++i) triangles.emplace_back(i, i + segs, i + segs + 1);

	// Caps
	int v0 = vertices.size();
	for (int i = 0; i < segs; ++i) {
		float yaw = i * twoPi / segs;
		auto v = Vec3f(std::cos(yaw) * radius, bot, std::sin(yaw) * radius);
		auto n = Vec3f(0, -1, 0);
		auto t = Vec2f(v.x * .5f + .5f, 1 - (v.z * .5f + .5f));
		vertices.emplace_back(v, n, t);
	}
	for (int i = 1; i < segs - 1; ++i) triangles.emplace_back(v0, v0 + i + 1, v0 + i);

	/*
		// Cap
		uint32_t v0 = vertices.size();
		for (int i = 0; i < segs; ++i) {
			float yaw = i % segs * twoPi / segs;
			auto n = Vec3f(std::cos(yaw), 0, std::sin(yaw));
			auto v = Vec3f(n.x * radius, bot, n.z * radius);
			auto t = Vec2f(n.x * 0.5f + 0.5f, n.z * 0.5f + 0.5f);
			vertices.emplace_back(v, n, t);
		}
		for (int i = 1; i < segs - 1; ++i) triangles.emplace_back(v0, v0 + i + 1, v0 + i);
	 */

	return createMesh(vertices, triangles, material);
}

Mesh* createTorusMesh(float outerRadius, float innerRadius, uint32_t outerSegs, uint32_t innerSegs, Material* material) {

	Vector<Vertex> vertices;
	Vector<Triangle> triangles;

	for (int outer = 0; outer <= outerSegs; ++outer) {

		auto sweep = Mat3f::yaw(outer * twoPi / outerSegs);

		for (int inner = 0; inner <= innerSegs; ++inner) {

			float r = inner * twoPi / innerSegs;
			float c = std::cos(r), s = std::sin(r);

			auto p = sweep * Vec3f(c * innerRadius + outerRadius, s * innerRadius, 0);
			auto n = sweep * Vec3f(c, s, 0);
			auto t = Vec2f(float(inner) / innerSegs, float(outer) / outerSegs);

			vertices.emplace_back(p, n, t);
		}
	}

	// Polygons
	for (int outer = 0; outer < outerSegs; ++outer) {
		auto v0 = outer * (innerSegs + 1);
		for (int inner = 0; inner < innerSegs; ++v0, ++inner) {
			auto v2 = v0 + innerSegs + 2;
			triangles.emplace_back(v0, v2, v0 + 1);
			triangles.emplace_back(v0, v2 - 1, v2);
		}
	}

	return createMesh(vertices, triangles, material);
}

} // namespace sgd
