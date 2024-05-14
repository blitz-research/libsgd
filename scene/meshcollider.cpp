#include "meshcollider.h"

#include "meshcolliderdata.h"

#include "model.h"

#include <graphics/exports.h>

namespace sgd {

namespace {

Map<CMesh*, MeshColliderDataPtr> g_datas;
}

MeshCollider::MeshCollider(Entity* entity, uint32_t colliderType, MeshColliderData* data)
	: Collider(entity, colliderType), m_data(data) {

	setLocalBounds(m_data->bounds());
}

Collider* MeshCollider::intersectRay(CLiner ray, real rradius, Contact& contact) {

	auto localRay = inverse(entity()->worldMatrix()) * ray;

	if (!m_data->intersectRay(localRay, rradius, contact)) return nullptr;

	contact.point = entity()->worldMatrix() * contact.point;
	contact.normal = entity()->worldBasis() * contact.normal;

	return this;
}

void MeshCollider::onUpdate(const CollisionSpace* space, uint32_t colliderMask, CollisionResponse response,
							Vector<Collision>& collisions) {
	// Meshes don't stop for no one!
}

} // namespace sgd
