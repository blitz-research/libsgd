#include "collider.h"

#include "collisionspace.h"
#include "scene.h"

namespace sgd {

Collider::Collider(Entity* entity, uint32_t collisionType, uint32_t collisionMask)
	: m_entity(entity), m_collisionType(collisionType), m_collisionMask(collisionMask) {
	SGD_ASSERT(collisionType<maxCollisionType);

	m_entity->enabledChanged.connect(this, [=](bool e) {
		if (e) {
			m_collisionNode = m_entity->scene()->collisionSpace()->insert(this);
		} else {
			m_entity->scene()->collisionSpace()->remove(m_collisionNode);
			m_collisionNode = nullptr;
		}
	});

	m_entity->invalidated.connect(this, [=]{
		if(m_collisionNode) m_collisionNode->invalidate();
	});

	if (m_entity->enabled()) m_collisionNode = m_entity->scene()->collisionSpace()->insert(this);
}

void Collider::setLocalBounds(CBoxf bounds) {
	m_localBounds = bounds;
	m_worldBounds = m_entity ? m_entity->worldMatrix() * Boxr(m_localBounds) : Boxr(m_localBounds);

	if(m_collisionNode) m_collisionNode->invalidate();
}

} // namespace sgd
