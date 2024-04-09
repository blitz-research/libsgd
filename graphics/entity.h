#pragma once

#include <geom/exports.h>

namespace sgd {

SGD_SHARED(Scene);

SGD_SHARED(Entity);

struct Entity : Shared {
	SGD_OBJECT_TYPE(Entity, Shared);

	Entity() = default;

	void setWorldPosition(CVec3f position) { // NOLINT (recursive)
		setLocalPosition(m_parent ? inverse(m_parent->worldMatrix()) * position : position);
	}

	void setWorldBasis(CMat3f basis) { // NOLINT (recursive)
		setLocalBasis(m_parent ? transpose(m_parent->worldBasis()) * basis : basis);
	}

	void setWorldScale(CVec3f scale) { // NOLINT (recursive)
		setLocalScale(m_parent ? (1.0f / m_parent->worldScale()) * scale : scale);
	}

	Vec3f worldPosition() const { // NOLINT (recursive)
		return m_parent ? m_parent->worldMatrix() * m_localMatrix.t : m_localMatrix.t;
	}

	Mat3f worldBasis() const { // NOLINT (recursive)
		return m_parent ? m_parent->worldBasis() * m_localBasis : m_localBasis;
	}

	Vec3f worldScale() const { // NOLINT (recursive)
		return m_parent ? m_parent->worldScale() * m_localScale : m_localScale;
	}

	void setLocalPosition(CVec3f position) {
		m_localMatrix.t = position;
		invalidateWorldMatrix();
	}

	void setLocalBasis(CMat3f basis) {
		m_localBasis = basis;
		invalidateLocalMatrix();
	}

	void setLocalScale(CVec3f scale) {
		m_localScale = scale;
		invalidateLocalMatrix();
	}

	Vec3f localPosition() const {
		return m_localMatrix.t;
	}

	Mat3f localBasis() const {
		return m_localBasis;
	}

	Vec3f localScale() const {
		return m_localScale;
	}

	void setWorldMatrix(CAffineMat4f matrix);

	AffineMat4f worldMatrix() const;

	void setLocalMatrix(CAffineMat4f matrix);

	AffineMat4f localMatrix() const;

	void setParent(Entity* parent);

	Entity* parent() const {
		return m_parent;
	}

	CVector<EntityPtr> children() const {
		return m_children;
	}

	void setEnabled(bool enabled);

	bool enabled() const {
		return m_enabled;
	}

	void setVisible(bool visible);

	bool visible() const {
		return m_visible;
	}

	Scene* scene() const {
		return m_scene;
	}

	Entity* copy() const {
		return onCopy();
	}

protected:
	explicit Entity(const Entity* that);

	virtual void onCreate(){};
	virtual void onEnable(){};
	virtual void onShow(){};
	virtual void onHide(){};
	virtual void onDisable(){};
	virtual void onDestroy(){};

	virtual Entity* onCopy() const = 0;

private:
	friend struct Scene;

	enum struct Dirty { none = 0, localMatrix = 1, worldMatrix = 2 };

	mutable AffineMat4f m_worldMatrix;
	mutable AffineMat4f m_localMatrix;
	mutable Mat3f m_localBasis;
	mutable Vec3f m_localScale{1};
	mutable Dirty m_dirty{Dirty::none};

	Entity* m_parent{};
	Vector<EntityPtr> m_children;

	bool m_enabled{true};
	bool m_visible{true};

	Scene* m_scene{};

	void invalidateWorldMatrix() const;
	void invalidateLocalMatrix() const;
};

} // namespace sgd
