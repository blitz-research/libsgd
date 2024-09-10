#include "meshcollider.h"

#include "meshcolliderdata.h"

#include <graphics/exports.h>

namespace sgd {

namespace {

Map<CMesh*, MeshColliderDataPtr> g_datas;
}

MeshCollider::MeshCollider(Entity* entity, uint32_t colliderType, MeshColliderData* data)
	: Collider(colliderType), m_data(data) {

	setLocalBounds(m_data->bounds());

	attach(entity);
}

Collider* MeshCollider::intersectRay(CLiner ray, float radius, Contact& contact) {

	auto localRay = inverse(entity()->worldMatrix()) * ray;

	if (!m_data->intersectRay(localRay, radius, contact)) return nullptr;

	contact.point = entity()->worldMatrix() * contact.point;
	contact.normal = entity()->worldBasis() * contact.normal;

	return this;
}

Collider* MeshCollider::intersectRay(CLiner ray, CVec3f radii, Contact& contact) {

	auto localRay = inverse(entity()->worldMatrix()) * ray;

	if (!m_data->intersectRay(localRay, radii, contact)) return nullptr;

	contact.point = entity()->worldMatrix() * contact.point;
	contact.normal = entity()->worldBasis() * contact.normal;

	return this;
}

void MeshCollider::onUpdate(const CollisionSpace* space, uint32_t colliderMask,
							Vector<Collision>& collisions) {
	// Meshes don't stop for no one!
}

} // namespace sgd
