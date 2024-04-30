#include "overlay.h"

#include "scene.h"

#include <graphics/exports.h>

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
}

void Overlay::onDestroy() {

	scene()->setRenderer(RendererType::overlay, nullptr);
}

void Overlay::onShow() {

	m_drawList->enabled = true;

	scene()->viewportSizeChanged.connect(this, [=](CVec2u size){
		m_drawList->projectionMatrix = Mat4f::ortho(0, (float)size.x, (float)size.y, 0, 0, 1);
	});
	m_drawList->projectionMatrix = Mat4f::ortho(0, (float)scene()->viewportSize().x, (float)scene()->viewportSize().y, 0, 0, 1);

	scene()->beginRender.connect(this, [=]{
		m_drawList->flush();
	});
}

void Overlay::onHide() {

	m_drawList->enabled = false;

	scene()->viewportSizeChanged.disconnect(this);

	scene()->beginRender.disconnect(this);
}

}
