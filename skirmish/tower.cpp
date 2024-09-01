#include "tower.h"

namespace skirmish {

namespace {

SGD_Mesh g_towerMesh;

auto init = onInitActorTypes([] { //

	g_towerMesh = sgd_LoadMesh("sgd://skirmish/tower.glb");
	sgd_SetMeshShadowsEnabled(g_towerMesh, SGD_TRUE);
});

} // namespace

Tower::Tower(float x, float z) : Actor(ActorType::tower) {
	m_entity = sgd_CreateModel(g_towerMesh);
	sgd_MoveEntity(m_entity,x,0,z);
	m_collider = sgd_CreateMeshCollider(m_entity, (int)ActorType::tower, SGD_NULL);
}

} // namespace skirmish
