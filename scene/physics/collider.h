#pragma once

#include "../scene/entity.h"
#include "contact.h"

namespace sgd {

constexpr uint32_t maxCollisionType = 31;

struct CollisionNode;
SGD_SHARED(CollisionSpace);
SGD_SHARED(Collider);

struct Collision {
	Liner ray;
	Contact contact;
	ColliderPtr collider;

	Collision(CLiner ray, CContact contact, Collider* collider):ray(ray),contact(contact),collider(collider){}
};

enum struct CollisionResponse {
	ignore = 0,
	stop,
	slide,
	slidexz,
};

struct Collider : EntityListener {
	SGD_OBJECT_TYPE(Collider, EntityListener);

	Collider(uint32_t colliderType);

	uint32_t colliderType() const {
		return m_colliderType;
	}

	CBoxf localBounds() const {
		return m_localBounds;
	}

	CBoxr worldBounds() const;

	void update(uint32_t colliderMask);

	virtual Collider* intersectRay(CLiner ray, float radius, Contact& contact) = 0;

	virtual Collider* intersectRay(CLiner ray, CVec3f radii, Contact& contact) = 0;

	CVector<Collision> collisions()const {
		return m_collisions;
	}

protected:

	virtual void onUpdate(const CollisionSpace* space, uint32_t colliderMask, Vector<Collision>& collisions) = 0;

	void setLocalBounds(CBoxf bounds);

private:
	uint32_t m_colliderType;
	Boxf m_localBounds;
	mutable Boxr m_worldBounds;
	mutable bool m_worldBoundsValid{};
	CollisionNode* m_collisionNode{};
	Vector<Collision> m_collisions;

	void onEnable() override;
	void onDisable() override;
	void onInvalidate() override;
};

}; // namespace sgd
