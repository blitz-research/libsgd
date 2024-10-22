#include "terrain.h"

#include "terrainrenderer.h"
#include "../scene/scenerenderer.h"

namespace sgd {

Terrain::Terrain() :m_bindings(new TerrainBindings()){
	init();
}

Terrain::Terrain(const Terrain* that) :m_bindings(that->m_bindings){
	init();
}

real Terrain::getHeight(real x, real z) const {
	auto v = inverse(worldMatrix()) * Vec3r(x,0,z);
	v.y = m_bindings->getLocalHeight((float)v.x,(float)v.z);
	return (worldMatrix() * v).y;
}

Terrain* Terrain::onCopy() const {
	return new Terrain(this);
}

void Terrain::onShow() {
	scene()->sceneRenderer()->terrainRenderer()->add(this);
}

void Terrain::onHide() {
	scene()->sceneRenderer()->terrainRenderer()->remove(this);
}

void Terrain::init() {
}

}
