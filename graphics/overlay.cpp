#include "overlay.h"

#include "scene.h"

namespace sgd {

Overlay::Overlay(const Overlay* that) : Entity(that){
}

Overlay* Overlay::onCopy() const {
	return new Overlay(this);
}

void Overlay::onCreate() {
	SGD_ASSERT(!scene()->getRenderer(RendererType::overlay));

	m_drawList = new DrawList();
	m_drawList->enabled = false;

	scene()->setRenderer(RendererType::overlay, m_drawList);

	scene()->beginRender.connect(this, [=]{
		m_drawList->flush();
	});
}

void Overlay::onDestroy() {
	scene()->setRenderer(RendererType::overlay, nullptr);
}

void Overlay::onShow() {
	m_drawList->enabled = true;
}

void Overlay::onHide() {
	m_drawList->enabled = false;
}

}
