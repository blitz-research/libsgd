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
