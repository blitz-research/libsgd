#include "player.h"

#include "bullet.h"

namespace skirmish {

namespace {

SGD_Mesh g_playerMesh;

SGD_Sound g_shoot;

float g_colliderRadius;
float g_colliderHeight;

auto init = onInitActorTypes([]{
	g_playerMesh = sgd_LoadMesh("sgd://skirmish/saucer.glb");
	g_colliderRadius = std::max(-sgd_GetMeshBoundsMinX(g_playerMesh), sgd_GetMeshBoundsMaxX(g_playerMesh));
	g_colliderHeight = std::max(-sgd_GetMeshBoundsMinY(g_playerMesh), sgd_GetMeshBoundsMaxY(g_playerMesh)) * 2;

	sgd_EnableCollisions((int)ActorType::player, (int)ActorType::tower, SGD_COLLISION_RESPONSE_SLIDE);

	g_shoot = sgd_LoadSound("~/Desktop/player-shoot.wav");
});

}

Player::Player() : Actor(ActorType::player) {

	m_entity = sgd_CreateModel(g_playerMesh);
	sgd_MoveEntity(m_entity, 0, 2.5f, 0);
	auto collider = sgd_CreateEllipsoidCollider(m_entity, (int)ActorType::player, g_colliderRadius, g_colliderHeight);

	m_camera = sgd_CreatePerspectiveCamera();
	sgd_SetEntityParent(m_camera, m_entity);
	sgd_MoveEntity(m_camera,0,.5f,0);
}

void Player::onUpdate() {
	if (sgd_IsKeyDown(SGD_KEY_LEFT)) {
		m_rvy += (1.5f - m_rvy) * .3f;
	} else if (sgd_IsKeyDown(SGD_KEY_RIGHT)) {
		m_rvy += (-1.5f - m_rvy) * .3f;
	} else {
		m_rvy *= .9f;
	}
	sgd_RotateEntity(m_entity, 0, m_rvy, 0);

	static constexpr SGD_Real speed = .125f;

	if (sgd_IsKeyDown(SGD_KEY_W)) {
		m_vz += (speed - m_vz) * .3f;
	} else if (sgd_IsKeyDown(SGD_KEY_S)) {
		m_vz += (-speed - m_vz) * .3f;
	} else {
		m_vz *= .9f;
	}
	sgd_MoveEntity(m_entity, 0,0,m_vz);

	if(sgd_IsKeyHit(SGD_KEY_SPACE)) {
		new Bullet(this);
		sgd_PlaySound(g_shoot);
	}
}

void Player::onDestroy() {
	sgd_DestroyEntity(m_camera);
}

} // namespace skirmish
