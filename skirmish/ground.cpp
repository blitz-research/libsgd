#include "ground.h"

namespace skirmish {

namespace {

SGD_Mesh g_groundMesh;

auto init = onInitActorTypes([] {

	auto material = sgd_CreatePBRMaterial();
	sgd_SetMaterialVector4f(material, "albedoColor4f", .25f, 1, 0, 1);

	g_groundMesh = sgd_CreateBoxMesh(-100, 0, -100, 100, 0, 100, material);
});

} // namespace

Ground::Ground() : Actor(ActorType::ground) {
	m_entity = sgd_CreateModel(g_groundMesh);
	sgd_CreateMeshCollider(m_entity, (int)ActorType::ground, SGD_NULL);
}

} // namespace skirmish
