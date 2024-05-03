#pragma once

#include <geom/exports.h>

namespace sgd {

SGD_SHARED(Scene);
SGD_SHARED(Collider);
SGD_SHARED(Entity);

struct Entity : Shared {
	SGD_OBJECT_TYPE(Entity, Shared);

	Entity();

	Property<bool> isEnabled{true};
	Property<bool> isVisible{true};

	Signal<bool> enabledChanged;
	Signal<bool> visibleChanged;

	Signal<> invalidated;

	bool enabled() const { // NOLINT (recursive)
		return isEnabled() && (!m_parent || m_parent->enabled());
	}

	bool visible() const { // NOLINT (recursive)
		return isVisible() && (!m_parent || m_parent->visible());
	}

	void setParent(Entity* parent);

	Entity* parent() const {
		return m_parent;
	}

	CVector<EntityPtr> children() const {
		return m_children;
	}

	void setCollider(Collider* collider);

	Collider* collider() const {
		return m_collider;
	}

	Scene* scene() const {
		return m_scene;
	}

	Entity* copy() const {
		return onCopy();
	}

	void setWorldPosition(CVec3r position) { // NOLINT (recursive)
		setLocalPosition(m_parent ? inverse(m_parent->worldMatrix()) * position : position);
	}

	void setWorldBasis(CMat3r basis) { // NOLINT (recursive)
		setLocalBasis(m_parent ? transpose(m_parent->worldBasis()) * basis : basis);
	}

	void setWorldScale(CVec3r scale) { // NOLINT (recursive)
		setLocalScale(m_parent ? (real(1) / m_parent->worldScale()) * scale : scale);
	}

	Vec3r worldPosition() const { // NOLINT (recursive)
		return m_parent ? m_parent->worldMatrix() * m_localMatrix.t : m_localMatrix.t;
	}

	Mat3r worldBasis() const { // NOLINT (recursive)
		return m_parent ? m_parent->worldBasis() * m_localBasis : m_localBasis;
	}

	Vec3r worldScale() const { // NOLINT (recursive)
		return m_parent ? m_parent->worldScale() * m_localScale : m_localScale;
	}

	void setLocalPosition(CVec3r position) {
		m_localMatrix.t = position;
		invalidateWorldMatrix();
	}

	void setLocalBasis(CMat3r basis) {
		m_localBasis = basis;
		invalidateLocalMatrix();
	}

	void setLocalScale(CVec3r scale) {
		m_localScale = scale;
		invalidateLocalMatrix();
	}

	Vec3r localPosition() const {
		return m_localMatrix.t;
	}

	Mat3r localBasis() const {
		return m_localBasis;
	}

	Vec3r localScale() const {
		return m_localScale;
	}

	void setWorldMatrix(CAffineMat4r matrix);

	CAffineMat4r worldMatrix() const;

	void setLocalMatrix(CAffineMat4r matrix);

	CAffineMat4r localMatrix() const;

	template <class FuncTy> void visitChildren(FuncTy func) {
		for (Entity* child : m_children) visitChild(func);
	}

	template <class FuncTy> bool visitChild(FuncTy func) {
		if (!func(this)) return false;
		for (Entity* child : m_children) visitChild(func);
		return true;
	}

protected:
	explicit Entity(const Entity* that);

	virtual void onCreate(){};
	virtual void onEnable(){};
	virtual void onShow(){};
	virtual void onHide(){};
	virtual void onDisable(){};
	virtual void onDestroy(){};
	virtual void onValidate(){};

	virtual Entity* onCopy() const = 0;

	void invalidate();

private:
	friend class Scene;

	enum struct Dirty { none = 0, localMatrix = 1, worldMatrix = 2 };

	mutable AffineMat4r m_worldMatrix;
	mutable AffineMat4r m_localMatrix;
	mutable Mat3r m_localBasis;
	mutable Vec3r m_localScale{1};
	mutable Dirty m_dirty{Dirty::none};

	Scene* m_scene{};
	bool m_invalid{};

	Entity* m_parent{};
	Vector<EntityPtr> m_children;

	ColliderPtr m_collider;

	void invalidateWorldMatrix();
	void invalidateLocalMatrix();
	void init();
	void create(Scene* scene);
	void destroy();
	void validate();
};

} // namespace sgd
