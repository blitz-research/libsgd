#include "model.h"

#include "scene.h"

#include "modelrenderer.h"

namespace sgd {

Model::Model(Vector<EntityPtr> bones, Vector<AnimationPtr> animations)
	: m_bones(std::move(bones)), m_animations(std::move(animations)) {
}

Model::Model(const Model* that)
	: Entity(that),					   //													//
	  mesh(that->mesh()),			   //
	  color(that->color()),			   //
	  m_bones(that->m_bones),		   //
	  m_animations(that->m_animations) // We need an AnimationSet
{

	// This is currently a bit crusty, we need to remap bone entities to their copies, assumes they've been copied by Entity
	// ctor 'in order'. We need a nicer way to do this ultimately.
	//
	Map<Entity*, Entity*> bonesMap;
	Function<void(const Entity*, const Entity*)> mapBones;
	mapBones = [&](const Entity* e0, const Entity* e1) {
		SGD_ASSERT(e0->children().size() == e1->children().size()); // should be same after copy!
		for (int i = 0; i < e0->children().size(); ++i) {
			auto c0 = e0->children()[i];
			auto c1 = e1->children()[i];
			bonesMap[c0] = c1;
			mapBones(c0, c1);
		}
	};
	mapBones(this, that);
	for (int i = 0; i < m_bones.size(); ++i) m_bones[i] = bonesMap[m_bones[i]];
}

Model* Model::onCopy() const {
	return new Model(this);
}

void Model::onCreate() {
	if (!scene()->getRenderer(RendererType::model)) scene()->setRenderer(RendererType::model, new ModelRenderer());
}

void Model::onShow() {
	scene()->getRenderer(RendererType::model)->as<ModelRenderer>()->add(this);
}

void Model::onHide() {
	scene()->getRenderer(RendererType::model)->as<ModelRenderer>()->remove(this);
}

void Model::animate(uint32_t index, float time, AnimationMode mode) {
	SGD_ASSERT(index < m_animations.size());
	auto anim = m_animations[index];

	switch (mode) {
	case AnimationMode::oneshot:
		time = std::min(std::max(time, 0.0f), anim->duration);
		break;
	case AnimationMode::loop:
		time = sgd::floorMod(time, anim->duration);
		break;
	case AnimationMode::pingpong:
		time = sgd::floorMod(time, anim->duration * 2);
		if (time >= anim->duration) time = anim->duration - (time - anim->duration);
		break;
	}

	for (auto seq : anim->sequences) {
		auto bone = m_bones[seq->boneIndex];

		auto pos = seq->evaluatePosition(time, bone->localMatrix().t);
		auto rot = seq->evaluateRotation(time, Quatf::rotation(bone->localMatrix().r));
		auto scl = seq->evaluateScale(time, scale(bone->localMatrix().r));

		auto matrix = AffineMat4f::translation(pos) * AffineMat4f{Mat3f::rotation(rot), {}} * AffineMat4f::scale(scl);
		bone->setLocalMatrix(matrix);
	}
}

} // namespace sgd
