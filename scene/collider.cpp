#include "collider.h"

#include "collisionspace.h"
#include "scene.h"

namespace sgd {

Collider::Collider(Entity* entity, uint32_t colliderType)
	: m_entity(entity), m_colliderType(colliderType) {

	SGD_ASSERT(colliderType<32);

	if (m_entity->enabled()) m_collisionNode = m_entity->scene()->collisionSpace()->insert(this);

	m_entity->addListener(this);
}

void Collider::setLocalBounds(CBoxf bounds) {
	m_localBounds = bounds;
	m_worldBoundsValid = false;
}

CBoxr Collider::worldBounds() const {
	if(m_worldBoundsValid) return m_worldBounds;
	m_worldBoundsValid = true;
	return m_worldBounds = m_entity ? m_entity->worldMatrix() * Boxr(m_localBounds) : Boxr(m_localBounds);
}

void Collider::update(uint32_t colliderMask, CollisionResponse response) {
	SGD_ASSERT(m_collisionNode);
	m_collisions.clear();
	onUpdate(m_collisionNode->space(), colliderMask, response, m_collisions);
}

void Collider::onEnable(Entity* entity) {
	m_collisionNode = m_entity->scene()->collisionSpace()->insert(this);
}

void Collider::onDisable(Entity* entity) {
	if(!m_collisionNode) return;

	m_entity->scene()->collisionSpace()->remove(m_collisionNode);
	m_collisionNode = nullptr;
}

void Collider::onInvalidate(Entity* entity) {
	m_worldBoundsValid = false;
}

} // namespace sgd
