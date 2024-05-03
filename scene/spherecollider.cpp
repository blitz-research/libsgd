#include "spherecollider.h"

#include "intersect.h"

namespace sgd {

SphereCollider::SphereCollider(Entity* entity, uint32_t collisionType, uint32_t collisionMask, float iradius)
	: Collider(entity, collisionType, collisionMask), radius(iradius) {

	this->radius.changed.connect(nullptr, [=](float r){
		setLocalBounds({-r, r});
	});
}

const Collider* SphereCollider::intersectRay(CLiner ray, real rradius, Contact& contact) const {

	if (!intersectRaySphere(ray, entity()->worldMatrix().t, rradius + radius(), contact)) return nullptr;

	contact.point -= contact.normal * rradius;

	return this;
}

} // namespace sgd
