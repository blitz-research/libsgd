#include "meshutil.h"

namespace sgd {

Boxf bounds(CMesh* mesh) {

	auto vp = mesh->vertices();

	Boxf bounds;

	for (uint32_t i = 0; i < mesh->vertexCount(); ++vp, ++i) bounds |= vp->position;

	return bounds;
}

void transform(Mesh* mesh, CAffineMat4f matrix) {

	auto vp = mesh->lockVertices(0, mesh->vertexCount());

	auto cof = cofactor(matrix.r);

	for (uint32_t i = 0; i < mesh->vertexCount(); ++i) {
		vp[i].position = matrix * vp[i].position;
		vp[i].normal = normalize(cof * vp[i].normal);
	}

	mesh->unlockVertices();
}

void transformTexCoords(Mesh* mesh, CVec2f scale, CVec2f offset) {

	auto vp = mesh->lockVertices(0, mesh->vertexCount());

	for (uint32_t i = 0; i < mesh->vertexCount(); ++i) {
		vp[i].texCoords = vp[i].texCoords * scale + offset;
	}

	mesh->unlockVertices();
}

void fit(Mesh* mesh, CBoxf box, bool uniform) {

	auto bnds = bounds(mesh);

	auto scale = size(box) / size(bnds);
	if (uniform) scale = Vec3f(std::min(scale.x, std::min(scale.y, scale.z)));

	auto m = Mat3f::scale(scale);
	auto t = center(box) - m * center(bnds);

	transform(mesh, {m, t});
}

void updateNormals(Mesh* mesh) {

	auto vp = mesh->lockVertices(0, mesh->vertexCount());
	auto tp = mesh->triangles();

	for (uint32_t i = 0; i < mesh->vertexCount(); ++i) {
		vp[i].normal = {};
	}

	for (uint32_t i = 0; i < mesh->triangleCount(); ++i) {
		auto& v0 = vp[tp[i].indices[0]];
		auto& v1 = vp[tp[i].indices[1]];
		auto& v2 = vp[tp[i].indices[2]];
		auto n = normalize(cross(v1.position - v0.position, v2.position - v0.position));
		v0.normal += n;
		v1.normal += n;
		v2.normal += n;
	}

	for (uint32_t i = 0; i < mesh->vertexCount(); ++i) {
		vp[i].normal = normalize(vp[i].normal);
	}

	mesh->unlockVertices();
}

void updateTangents(Mesh* mesh) {

	auto vp = mesh->lockVertices(0, mesh->vertexCount());

	Vector<Vec3f> tan1(mesh->vertexCount());
	Vector<Vec3f> tan2(mesh->vertexCount());

	for (auto& surf : mesh->surfaces()) {

		auto tp = mesh->triangles() + surf.firstTriangle;

		for (uint32_t i = 0; i < surf.triangleCount; ++i) {

			auto& tri = tp[i];

			auto i0 = tri.indices[0];
			auto i1 = tri.indices[1];
			auto i2 = tri.indices[2];

			auto& v0 = vp[i0];
			auto& v1 = vp[i1];
			auto& v2 = vp[i2];

			auto x1 = v1.position.x - v0.position.x;
			auto x2 = v2.position.x - v0.position.x;
			auto y1 = v1.position.y - v0.position.y;
			auto y2 = v2.position.y - v0.position.y;
			auto z1 = v1.position.z - v0.position.z;
			auto z2 = v2.position.z - v0.position.z;

			auto s1 = v1.texCoords.x - v0.texCoords.x;
			auto s2 = v2.texCoords.x - v0.texCoords.x;
			auto t1 = v1.texCoords.y - v0.texCoords.y;
			auto t2 = v2.texCoords.y - v0.texCoords.y;

			auto r = 1 / (s1 * t2 - s2 * t1);

			auto sv = Vec3f((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
			auto tv = Vec3f((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

			tan1[i0] += sv;
			tan1[i1] += sv;
			tan1[i2] += sv;

			tan2[i0] += tv;
			tan2[i1] += tv;
			tan2[i2] += tv;
		}
	}

	for (uint32_t i = 0; i < mesh->vertexCount(); ++i) {
		auto& v = vp[i];
		auto& n = v.normal;
		auto& t = tan1[i];
		auto tv = normalize(t - n * dot(n, t));
		float w = dot(cross(n, t), tan2[i]) < 0 ? -1 : 1;
		v.tangent = {tv, w};
	}

	mesh->unlockVertices();
}

} // namespace sgd
