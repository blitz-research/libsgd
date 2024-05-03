#pragma once

#include "entity.h"
#include "contact.h"

namespace sgd {

constexpr uint32_t maxCollisionType = 31;

struct CollisionNode;

struct Collider : Shared {
	SGD_OBJECT_TYPE(Collider, Shared);

	Collider(Entity* entity, uint32_t collisionType, uint32_t collisionMask);

	Entity* entity() const {
		return m_entity;
	}

	uint32_t collisionType() const {
		return m_collisionType;
	}

	uint32_t collisionMask() const {
		return m_collisionMask;
	}

	CBoxf localBounds() const {
		return m_localBounds;
	}

	CBoxr worldBounds() const {
		return m_worldBounds;
	}

	virtual const Collider* intersectRay(CLiner ray, real rayRadius, Contact& contact) const = 0;

protected:
	void setLocalBounds(CBoxf bounds);

private:
	Entity* m_entity;
	uint32_t m_collisionType;
	uint32_t m_collisionMask;
	Boxf m_localBounds;
	Boxr m_worldBounds;
	CollisionNode* m_collisionNode{};
};

}; // namespace sgd
