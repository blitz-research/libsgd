#include "meshcolliderdata.h"

#include "intersect.h"

#include <graphics/exports.h>

namespace sgd {

struct MeshColliderData::Node {
	Boxr const bounds;
	Node* const lhs{};
	Node* const rhs{};
	Vector<Triangle> triangles;

	Node(CBoxr bounds, Vector<Triangle> triangles) : bounds(bounds), triangles(std::move(triangles)) {
	}

	Node(CBoxr bounds, Node* lhs, Node* rhs) : bounds(bounds), lhs(lhs), rhs(rhs) {
	}

	bool intersectRay(CLiner ray, CBoxr rayBounds, float radius, Contact& contact, CVector<Vec3f> vertices) const;

	bool intersectRay(CLiner ray, CBoxr rayBounds, CVec3f invRadii, Contact& contact, CVector<Vec3f> vertices) const;
};

MeshColliderData::MeshColliderData(CMesh* mesh) {

	static_assert(sizeof(Triangle) == sizeof(sgd::Triangle));

	m_vertices.resize(mesh->vertexCount());
	auto vp = mesh->vertices();
	for (auto& vert : m_vertices) vert = vp++->position;

	uint32_t triCount = 0;
	for (Surface* surf : mesh->surfaces()) triCount += surf->triangleCount();

	Vector<Triangle> triangles(triCount);
	auto dstp = triangles.data();
	for (Surface* surf : mesh->surfaces()) {
		sgd::copy((sgd::Triangle*)dstp, surf->triangles(), surf->triangleCount());
		dstp += surf->triangleCount();
	}

	SGD_LOG << "Creating mesh collision tree...";

	m_rootNode = createNode(std::move(triangles));

	m_bounds = m_rootNode->bounds;

	SGD_LOG << "Collision tree created, bounds:" << m_bounds;
}

MeshColliderData::Node* MeshColliderData::createNode(Vector<Triangle> triangles) { // NOLINT (recursive)

	Boxf bounds;
	for (auto& tri : triangles) {
		bounds |= m_vertices[tri.indices[0]];
		bounds |= m_vertices[tri.indices[1]];
		bounds |= m_vertices[tri.indices[2]];
	}

	if (triangles.size() <= 128) return new Node(bounds, std::move(triangles));

	auto size = sgd::size(bounds);

	if (size.x > size.y && size.x > size.z) {
		std::sort(triangles.begin(), triangles.end(), [=](const Triangle& lhs, const Triangle& rhs) {
			return m_vertices[lhs.indices[0]].x + m_vertices[lhs.indices[1]].x + m_vertices[lhs.indices[2]].x <
				   m_vertices[rhs.indices[0]].x + m_vertices[rhs.indices[1]].x + m_vertices[rhs.indices[2]].x;
		});
	} else if (size.y > size.z) {
		std::sort(triangles.begin(), triangles.end(), [=](const Triangle& lhs, const Triangle& rhs) {
			return m_vertices[lhs.indices[0]].y + m_vertices[lhs.indices[1]].y + m_vertices[lhs.indices[2]].y <
				   m_vertices[rhs.indices[0]].y + m_vertices[rhs.indices[1]].y + m_vertices[rhs.indices[2]].y;
		});
	} else {
		std::sort(triangles.begin(), triangles.end(), [=](const Triangle& lhs, const Triangle& rhs) {
			return m_vertices[lhs.indices[0]].z + m_vertices[lhs.indices[1]].z + m_vertices[lhs.indices[2]].z <
				   m_vertices[rhs.indices[0]].z + m_vertices[rhs.indices[1]].z + m_vertices[rhs.indices[2]].z;
		});
	}

	auto mid = triangles.begin() + triangles.size() / 2;

	Node* lhs = createNode({triangles.begin(), mid});
	Node* rhs = createNode({mid, triangles.end()});

	return new Node(bounds, lhs, rhs);
}

bool MeshColliderData::intersectRay(CLiner ray, float radius, Contact& contact) const {

	Boxr rayBounds(ray.o);
	rayBounds |= ray * contact.time;
	rayBounds.min -= radius + boundsPadding;
	rayBounds.max += radius + boundsPadding;

	//	SGD_LOG << ray << rayBounds;

	return m_rootNode->intersectRay(ray, rayBounds, radius, contact, m_vertices);
}

bool MeshColliderData::intersectRay(CLiner ray, CVec3f radii, Contact& contact) const {

	auto rradii=Vec3r(radii);

	Boxr rayBounds(ray.o);
	rayBounds |= ray * contact.time;
	rayBounds.min -= rradii + boundsPadding;
	rayBounds.max += rradii + boundsPadding;

	auto invRadii = (real)1 / rradii;
	Liner invRay(ray.o * invRadii, ray.d * contact.time * invRadii);

	Contact invContact = contact;
	invContact.time = length(invRay.d);
	invRay.d = normalize(invRay.d);

//	if (!m_rootNode->intersectRay(invRay, rayBounds, 1, invContact, m_vertices)) return false;
	if (!m_rootNode->intersectRay(invRay, rayBounds, invRadii, invContact, m_vertices)) return false;

	contact.point = invContact.point * rradii;
	contact.normal = normalize(cofactor(Mat3<real>::scale(rradii)) * invContact.normal);
	contact.time = length(invRay.d * invContact.time * rradii);

	return true;
}

bool MeshColliderData::Node::intersectRay(CLiner ray, CBoxr rayBounds, float radius, Contact& contact, // NOLINT (recursive)
										  CVector<Vec3f> vertices) const {

	if (!intersects(rayBounds, bounds)) return false;

	if (triangles.empty()) {
		return lhs->intersectRay(ray, rayBounds, radius, contact, vertices) |
			   rhs->intersectRay(ray, rayBounds, radius, contact, vertices);
	}

	bool collision = false;

	for (auto& tri : triangles) {
		const auto& v0 = vertices[tri.indices[0]];
		const auto& v1 = vertices[tri.indices[2]];
		const auto& v2 = vertices[tri.indices[1]];

		Boxr triBounds(v0);
		triBounds |= v1;
		triBounds |= v2;

		if (!intersects(rayBounds, triBounds)) continue;

		SGD_LOG << "radius"<<radius;
		collision |= intersectRayTriangle(ray, radius, v0, v1, v2, contact);
	}
	return collision;
}

bool MeshColliderData::Node::intersectRay(CLiner ray, CBoxr rayBounds, CVec3f invRadii, Contact& contact, // NOLINT (recursive)
										  CVector<Vec3f> vertices) const {

	if (!intersects(rayBounds, bounds)) return false;

	if (triangles.empty()) {
		return lhs->intersectRay(ray, rayBounds, invRadii, contact, vertices) | //
			   rhs->intersectRay(ray, rayBounds, invRadii, contact, vertices);
	}

	bool collision = false;

	for (auto& tri : triangles) {
		const auto& v0 = vertices[tri.indices[0]];
		const auto& v1 = vertices[tri.indices[2]];
		const auto& v2 = vertices[tri.indices[1]];

		Boxr triBounds(v0);
		triBounds |= v1;
		triBounds |= v2;

		if (!intersects(rayBounds, triBounds)) continue;

		collision |= intersectRayTriangle(ray, 1, v0 * invRadii, v1 * invRadii, v2 * invRadii, contact);
	}
	return collision;
}

MeshColliderData* getOrCreateMeshColliderData(CMesh* mesh) {
	static Map<CMesh*, MeshColliderDataPtr> g_datas;

	auto it = g_datas.find(mesh);

	if (it != g_datas.end()) return it->second;

	auto data = new MeshColliderData(mesh);
	g_datas.insert(std::make_pair(mesh, data));
	mesh->deleted.connect(nullptr, [=] {
		//		auto it = g_datas.find(mesh);
		//		SGD_ASSERT(it !=g_datas.end());
		//		g_datas.erase(it);
	});

	return data;
}

} // namespace sgd
