#include "collider.h"

#include "../scene/scene.h"
#include "collisionspace.h"

namespace sgd {

Collider::Collider(uint32_t colliderType) : m_colliderType(colliderType) {
	if (colliderType > 31) SGD_ERROR("Collider type must be in the range 0..31");
}

void Collider::setLocalBounds(CBoxf bounds) {
	m_localBounds = bounds;
	m_worldBoundsValid = false;
}

CBoxr Collider::worldBounds() const {
	if (m_worldBoundsValid) return m_worldBounds;
	m_worldBoundsValid = true;
	return m_worldBounds = entity() ? entity()->worldMatrix() * Boxr(m_localBounds) : Boxr(m_localBounds);
}

void Collider::update(uint32_t colliderMask) {
	m_collisions.clear();
	onUpdate(m_collisionNode->space(), colliderMask, m_collisions);
}

void Collider::onEnable() {
	m_collisionNode = entity()->scene()->collisionSpace()->insert(this);
}

void Collider::onDisable() {
	if (!m_collisionNode) return;

	entity()->scene()->collisionSpace()->remove(m_collisionNode);
	m_collisionNode = nullptr;
}

void Collider::onInvalidate() {
	m_worldBoundsValid = false;
}

} // namespace sgd
