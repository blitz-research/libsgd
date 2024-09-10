#include "bullet.h"

#include "player.h"

#include <cmath>

namespace skirmish {

namespace {

constexpr float bulletRadius = .75f;

SGD_Mesh g_bulletMesh;

SGD_Sound g_bounce;

auto init = onInitActorTypes([] {
	auto material = sgd_CreatePrelitMaterial();
	sgd_SetMaterialBlendMode(material, SGD_BLEND_MODE_OPAQUE);
	sgd_SetMaterialVector4f(material, "albedoColor4f", 0, .5f, 2.0, 1);

	g_bulletMesh = sgd_CreateSphereMesh(bulletRadius, 24, 12, material);

	// Bullets collide with towers....
	sgd_EnableCollisions((int)ActorType::bullet, (int)ActorType::tower, SGD_COLLISION_RESPONSE_SLIDE);

	g_bounce = sgd_LoadSound("~/Desktop/bounce.wav");
});

} // namespace

Bullet::Bullet(Player* player) : Actor(ActorType::bullet) {

	m_entity = sgd_CreateModel(g_bulletMesh);
	sgd_SetEntityParent(m_entity, player->entity());
	sgd_SetEntityParent(m_entity, SGD_NULL);
	m_collider = sgd_CreateSphereCollider(m_entity, (int)ActorType::bullet, bulletRadius);

	auto light = sgd_CreatePointLight();
	sgd_SetEntityParent(light, m_entity);
	sgd_SetLightShadowsEnabled(light, SGD_TRUE);
	sgd_SetLightColor(light, 0, .5, 2.0, 1);
	sgd_SetLightRange(light, 25);
}

void Bullet::onUpdate() {
	if (!--m_timeout) {
		destroy();
		return;
	}

	if (sgd_GetCollisionCount(m_collider)) {

		// Get collision normal
		auto nx = sgd_GetCollisionNX(m_collider, 0);
		auto ny = sgd_GetCollisionNY(m_collider, 0);
		auto nz = sgd_GetCollisionNZ(m_collider, 0);

		// Get bullet vector...
		auto vx = sgd_GetEntityKX(m_entity);
		auto vy = sgd_GetEntityKY(m_entity);
		auto vz = sgd_GetEntityKZ(m_entity);

		// reflect vector off plane
		auto ndotv = nx * vx + ny * vy + nz * vz;
		vx -= nx * ndotv * 2;
		vy -= ny * ndotv * 2;
		vz -= nz * ndotv * 2;

		auto yaw = -std::atan2(vx, vz);

		sgd_SetEntityRotation(m_entity, 0, yaw * radiansToDegrees, 0);

		sgd_Play3DSound(g_bounce, m_entity);
	}

	static constexpr SGD_Real speed = 0.75f;
	sgd_MoveEntity(m_entity, 0, 0, speed);
}

} // namespace skirmish
