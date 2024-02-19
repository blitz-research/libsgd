#include "meshprimitives.h"

#include "meshutil.h"

namespace sgd {

namespace {

Mesh* createMesh(Vector<Vertex> vertices, Vector<Triangle> triangles, Material* material) {

	auto mesh = new Mesh(vertices.data(), vertices.size(), triangles.data(), triangles.size(), MeshFlags::tangentsEnabled);

	mesh->addSurface({material, 0, (uint32_t)triangles.size()});

	updateTangents(mesh);

	return mesh;
}

} // namespace

Mesh* createBoxMesh(CBoxf box, Material* material) {

	Vector<Vertex> vertices;
	Vector<Triangle> triangles;

	auto sz = size(box);
	Vec3f verts[]{{-sz.x / 2, sz.y / 2, -sz.z / 2},	 {sz.x / 2, sz.y / 2, -sz.z / 2}, {sz.x / 2, -sz.y / 2, -sz.z / 2},
				  {-sz.x / 2, -sz.y / 2, -sz.z / 2}, {-sz.x / 2, sz.y / 2, sz.z / 2}, {sz.x / 2, sz.y / 2, sz.z / 2},
				  {sz.x / 2, -sz.y / 2, sz.z / 2},	 {-sz.x / 2, -sz.y / 2, sz.z / 2}};

	Vec3f norms[]{{0, 0, -1}, {1, 0, 0}, {0, 0, 1}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}};

	uint32_t faces[]{0, 1, 2, 3, 1, 5, 6, 2, 5, 4, 7, 6, 4, 0, 3, 7, 4, 5, 1, 0, 3, 2, 6, 7};

	for (auto i = 0u; i < 24; i += 4) {
		CVec3f norm = norms[i / 4];
		vertices.emplace_back(verts[faces[i + 0]], norm, Vec2f(0, 0));
		vertices.emplace_back(verts[faces[i + 1]], norm, Vec2f(1, 0));
		vertices.emplace_back(verts[faces[i + 2]], norm, Vec2f(1, 1));
		vertices.emplace_back(verts[faces[i + 3]], norm, Vec2f(0, 1));
		triangles.emplace_back(i, i + 1, i + 2);
		triangles.emplace_back(i, i + 2, i + 3);
	}

	return createMesh(std::move(vertices), std::move(triangles), material);
}

Mesh* createSphereMesh(float radius, uint32_t xSegs, uint32_t ySegs, Material* material) {

	Vector<Vertex> vertices;
	Vector<Triangle> triangles;

	for (auto i = 0u; i < xSegs; ++i) vertices.emplace_back(Vec3f(0, radius, 0), Vec2f((i + 0.5f) / xSegs, 0));

	for (auto j = 1u; j < ySegs; ++j) {
		float pitch = j * pi / ySegs - halfPi;
		for (auto i = 0u; i <= xSegs; ++i) {
			float yaw = i * twoPi / xSegs;
			auto p = Mat3f::rotation({pitch, yaw, 0}).k * radius;
			vertices.emplace_back(p, Vec2f(float(i) / xSegs, float(j) / ySegs));
		}
	}

	for (auto i = 0u; i < xSegs; ++i) vertices.emplace_back(Vec3f{0, -radius, 0}, Vec2f{(i + 0.5f) / xSegs, 1});

	for (auto& v : vertices) v.normal = normalize(v.position);

	// Polygons
	for (auto i = 0u; i < xSegs; ++i) triangles.emplace_back(i, i + xSegs + 1, i + xSegs);

	for (auto j = 1u; j < ySegs - 1; ++j) {
		for (auto i = 0u; i < xSegs; ++i) {
			auto v0 = j * (xSegs + 1) + i - 1;
			auto v2 = v0 + xSegs + 2;
			triangles.emplace_back(v0, v0 + 1, v2);
			triangles.emplace_back(v0, v2, v2 - 1);
		}
	}

	for (auto i = 0u; i < xSegs; ++i) {
		auto v0 = (xSegs + 1) * (ySegs - 1) + i - 1;
		triangles.emplace_back(v0, v0 + 1, v0 + xSegs + 1);
	}

	return createMesh(std::move(vertices), std::move(triangles), material);
}

Mesh* createCylinderMesh(float length, float radius, uint32_t segs, Material* material) {

	Vector<Vertex> vertices;
	Vector<Triangle> triangles;

	float top = length, bot = 0;

	// Sides
	for (auto i = 0u; i <= segs; ++i) {
		float yaw = (i % segs) * twoPi / segs;
		auto v = Vec3f(cosf(yaw) * radius, top, sinf(yaw) * radius);
		auto n = normalize(Vec3f(v.x, 0, v.z));
		auto t = Vec2f(float(i) / segs, 0);
		vertices.emplace_back(v, n, t);
		v.y = bot;
		t.y = 1;
		vertices.emplace_back(v, n, t);
	}
	for (auto i = 0u; i < segs; ++i) {
		triangles.emplace_back(i * 2, i * 2 + 2, i * 2 + 3);
		triangles.emplace_back(i * 2, i * 2 + 3, i * 2 + 1);
	}

	// Caps
	int v0 = vertices.size();
	for (auto i = 0u; i < segs; ++i) {
		float yaw = i * twoPi / segs;
		auto v = Vec3f(cosf(yaw) * radius, top, sinf(yaw) * radius);
		auto n = Vec3f(0, 1, 0);
		auto t = Vec2f(v.x * .5f + .5f, 1 - (v.z * .5f + .5f));
		vertices.emplace_back(v, n, t);
		v.y = bot;
		n.y = -n.y;
		vertices.emplace_back(v, n, t);
	}
	for (auto i = 1u; i < segs - 1; ++i) {
		triangles.emplace_back(v0 + (i + 1) * 2, v0 + i * 2, v0);
		triangles.emplace_back(v0 + i * 2 + 1, v0 + (i + 1) * 2 + 1, v0 + 1);
	}

	return createMesh(std::move(vertices), std::move(triangles), material);
}

Mesh* createConeMesh(float length, float radius, uint32_t segs, Material* material) {

	Vector<Vertex> vertices;
	Vector<Triangle> triangles;

	float top = length, bot = 0;

	// Peak
	for (auto i = 0u; i < segs; ++i) {
		vertices.emplace_back(Vec3f(0, top, 0), Vec3f(0, 1, 0), Vec2f((i + 0.f) / segs, 0));
	}

	// Sides
	for (int i = 0; i <= segs; ++i) {
		float yaw = (i % segs) * twoPi / segs;
		auto n = Vec3f(cosf(yaw), 0, sinf(yaw));
		auto v = Vec3f(n.x * radius, bot, n.z * radius);
		auto t = Vec2f(float(i) / segs, 1);
		vertices.emplace_back(v, n, t);
	}
	for (auto i = 0u; i < segs; ++i) triangles.emplace_back(i, i + segs + 1, i + segs);

	// Cap
	uint32_t v0 = vertices.size();
	for (auto i = 0u; i < segs; ++i) {
		float yaw = (i % segs) * twoPi / segs;
		auto n = Vec3f(cosf(yaw), 0, sinf(yaw));
		auto v = Vec3f(n.x * radius, bot, n.z * radius);
		auto t = Vec2f(n.x * 0.5f + 0.5f, n.z * 0.5f + 0.5f);
		vertices.emplace_back(v, n, t);
	}
	for (auto i = 1u; i < segs - 1; ++i) triangles.emplace_back(v0, v0 + i, v0 + i + 1);

	return createMesh(std::move(vertices), std::move(triangles), material);
}

Mesh* createTorusMesh(float outerRadius, float innerRadius, uint32_t outerSegs, uint32_t innerSegs,
						Material* material) {

	Vector<Vertex> vertices;
	Vector<Triangle> triangles;

	for (auto outer = 0u; outer <= outerSegs; ++outer) {

		auto sweep = Mat3f::yaw(outer * twoPi / outerSegs);

		for (auto inner = 0u; inner <= innerSegs; ++inner) {

			float r = inner * twoPi / innerSegs;
			float c = std::cos(r), s = std::sin(r);

			auto p = sweep * Vec3f(c * innerRadius + outerRadius, s * innerRadius, 0);
			auto n = sweep * Vec3f(c, s, 0);
			auto t = Vec2f(float(inner) / innerSegs, float(outer) / outerSegs);

			vertices.emplace_back(p, n, t);
		}
	}

	// Polygons
	for (auto outer = 0u; outer < outerSegs; ++outer) {
		auto v0 = outer * (innerSegs + 1);
		for (auto inner = 0u; inner < innerSegs; ++v0, ++inner) {
			auto v2 = v0 + innerSegs + 2;
			triangles.emplace_back(v0, v0 + 1, v2);
			triangles.emplace_back(v0, v2, v2 - 1);
		}
	}

	return createMesh(std::move(vertices), std::move(triangles), material);
}

} // namespace sgd
