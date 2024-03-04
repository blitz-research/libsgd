#pragma once

#include "animation.h"
#include "entity.h"
#include "mesh.h"

namespace sgd {

SGD_SHARED(Model);

struct Model : Entity {
	SGD_OBJECT_TYPE(Model, Entity);

	Model() = default;

	Model(Vector<EntityPtr> bones, Vector<AnimationPtr> animations);

	explicit Model(const Model* that);;

	Property<CMeshPtr> mesh;

	Property<Vec4f> color{Vec4f{1, 1, 1, 1}};

	void animate(uint32_t index, float time, AnimationMode mode);

private:
	Vector<EntityPtr> m_bones;
	Vector<AnimationPtr> m_animations;

	void onCreate() override;
	void onShow() override;
	void onHide() override;

	Model* onCopy() const override;
};

} // namespace sgd
