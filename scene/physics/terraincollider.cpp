#include "terraincollider.h"

namespace sgd {

TerrainCollider::TerrainCollider(Entity* entity, uint32_t colliderType, TerrainBindings* bindings)
	: Collider(colliderType), m_bindings(bindings){
	
	auto size = (float)m_bindings->size();
	
	auto bounds = Boxf(-size*.5f, size*.5f);
	bounds.min *= entity->worldScale();
	bounds.max *= entity->worldScale();
	
	setLocalBounds(bounds);

	attach(entity);
}

Collider* TerrainCollider::intersectRay(CLiner ray, float radius, Contact& contact) {
	
	auto localRay = inverse(entity()->worldMatrix()) * ray;

	//if (!m_data->intersectRay(localRay, radius, contact)) return nullptr;

	contact.point = entity()->worldMatrix() * contact.point;
	contact.normal = entity()->worldBasis() * contact.normal;

	return this;
}

Collider* TerrainCollider::intersectRay(CLiner ray, CVec3f radii, Contact& contact) {

	auto localRay = inverse(entity()->worldMatrix()) * ray;

	//if (!m_data->intersectRay(localRay, radii, contact)) return nullptr;

	contact.point = entity()->worldMatrix() * contact.point;
	contact.normal = entity()->worldBasis() * contact.normal;

	return this;
}

void TerrainCollider::onUpdate(const CollisionSpace* space, uint32_t colliderMask, Vector<Collision>& collisions) {
}

}
