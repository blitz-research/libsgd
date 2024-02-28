#include "model.h"

#include "scene.h"

#include "modelrenderer.h"

namespace sgd {

void Model::onCreate() {
	if(!scene()->getRenderer(RendererType::model)) scene()->setRenderer(RendererType::model, new ModelRenderer());
}

void Model::onShow() {
	scene()->getRenderer(RendererType::model)->as<ModelRenderer>()->add(this);
}

void Model::onHide() {
	scene()->getRenderer(RendererType::model)->as<ModelRenderer>()->remove(this);
}

} // namespace sgd
