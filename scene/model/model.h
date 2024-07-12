#pragma once

#include "../scene/entity.h"
#include "animation.h"

namespace sgd {

SGD_SHARED(Mesh);
SGD_SHARED(Model);

struct Joint {
	AffineMat4f inverseBindMatrix;
	int bone{-1};
};

struct Model : Entity {
	SGD_OBJECT_TYPE(Model, Entity);

	Property<MeshPtr> mesh;
	Property<Vec4f> color{Vec4f(1)};

	Model() = default;
	explicit Model(Mesh* mesh);
	explicit Model(const Model* that);
	Model(Vector<EntityPtr> bones, Vector<AnimationPtr> animations, Vector<Joint> joints);

	void animate(uint32_t index, float time, AnimationMode mode, float weight);

	CVector<AffineMat4r> jointMatrices() const {
		return m_jointMatrices;
	}

private:
	Vector<AnimationPtr> m_animations;
	Vector<EntityPtr> m_bones;
	Vector<Joint> m_joints;
	bool m_skinned{};
	Vector<AffineMat4r> m_jointMatrices;

	Model* onCopy() const override;

	void onShow() override;
	void onHide() override;
};

} // namespace sgd
