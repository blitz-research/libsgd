#include "meshcollider.h"

#include "meshcolliderdata.h"

namespace sgd {

MeshCollider::MeshCollider(Entity* entity, uint32_t collisionType, uint32_t collisionMask, MeshColliderData* data):
Collider(entity, collisionType, collisionMask), m_data(data) {

	setLocalBounds(m_data->bounds());
}

const Collider* MeshCollider::intersectRay(CLiner ray, real rradius, Contact& contact) const {

	auto localRay = inverse(entity()->worldMatrix()) * ray;

	if (!m_data->intersectRay(localRay, rradius, contact)) return nullptr;

	contact.point = entity()->worldMatrix() * contact.point;
	contact.normal = entity()->worldBasis() * contact.normal;

	return this;
}

}
