#include "saucer.h"

#include "game.h"

namespace skirmish {

namespace {

SGD_Mesh g_saucerMesh;

SGD_Sound g_saucerShoot;

float g_colliderRadius;
float g_colliderHeight;

auto init = onInitActorTypes([] {
	g_saucerMesh = sgd_LoadMesh("sgd://skirmish/saucer.glb");
	g_colliderRadius = std::max(-sgd_GetMeshBoundsMinX(g_saucerMesh), sgd_GetMeshBoundsMaxX(g_saucerMesh));
	g_colliderHeight = std::max(-sgd_GetMeshBoundsMinY(g_saucerMesh), sgd_GetMeshBoundsMaxY(g_saucerMesh)) * 2;

	sgd_EnableCollisions((int)ActorType::player, (int)ActorType::tower, SGD_COLLISION_RESPONSE_SLIDE);

	g_saucerShoot = sgd_LoadSound("sgd://skirmish/saucer-shoot.wav");
});

} // namespace

Saucer::Saucer() : Actor(ActorType::saucer) {

	float x = std::floor(rnd(Game::numCells)) * Game::cellSize + Game::cellSize * .5f;
	float z = std::floor(rnd(Game::numCells)) * Game::cellSize + Game::cellSize * .5f;

	m_entity = sgd_CreateModel(g_saucerMesh);
	sgd_MoveEntity(m_entity, x, 12.5f, z);
	m_velY = -.25f;
}

void Saucer::onUpdate() {

	if (!--m_changeDirTimeout) {
		SGD_Real dx, dz;
		if (rnd() >= .5f) {
			int ix = (int)rnd(Game::numCells - 1);
			int tx = (int)(sgd_GetEntityX(m_entity) / Game::cellSize);
//			printf("### %i %i\n",ix, tx);fflush(stdout);
			if (ix >= tx) ix = (ix + 1) % Game::numCells;
			dx = ix * Game::cellSize + Game::cellSize * .5f;

			dz = sgd_GetEntityZ(m_entity);
		} else {
			int iz = (int)rnd(Game::numCells - 1);
			int tz = (int)(sgd_GetEntityZ(m_entity) / Game::cellSize);
			if (iz >= tz) iz = (iz + 1) % Game::numCells;
			dz = iz * Game::cellSize + Game::cellSize * .5f;
			dx = sgd_GetEntityX(m_entity);
		}
		dx -= sgd_GetEntityX(m_entity);
		dz -= sgd_GetEntityZ(m_entity);
		SGD_Real speed = .25f;
		SGD_Real time = std::max(std::abs(dx), std::abs(dz)) / speed;
		m_velX = dx / time;
		m_velZ = dz / time;
		m_changeDirTimeout = (int)time;
//		printf("### %f %f %f %f %f\n", dx, dz, m_velX, m_velZ, time);fflush(stdout);
	}
	sgd_MoveEntity(m_entity, m_velX, m_velY, m_velZ);
	if (sgd_GetEntityY(m_entity) < 2.5f) {
		sgd_MoveEntity(m_entity, 0, -m_velY, 0);
		m_velY = 0;
	}
}

} // namespace skirmish
