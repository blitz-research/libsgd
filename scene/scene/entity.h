#pragma once

#include <geom/exports.h>

namespace sgd {

SGD_SHARED(Scene);
SGD_SHARED(EntityListener);
SGD_SHARED(Entity);

struct Entity : Shared {
	SGD_OBJECT_TYPE(Entity, Shared);

	Entity() = default;

	void reset();

	void setIsEnabled(bool isEnabled);

	bool isEnabled() const {
		return m_isEnabled;
	}

	bool enabled() const { // NOLINT (recursive)
		return isEnabled() && (!m_parent || m_parent->enabled()) && m_scene;
	}

	void setIsVisible(bool isVisible);

	bool isVisible() const {
		return m_isVisible;
	}

	bool visible() const { // NOLINT (recursive)
		return isVisible() && (!m_parent || m_parent->visible()) && m_scene;
	}

	void setParent(Entity* parent);

	Entity* parent() const {
		return m_parent;
	}

	CVector<EntityPtr> children() const {
		return m_children;
	}

	void setName(CString name);

	CString name() const {
		return m_name;
	}

	CVector<EntityListenerPtr> listeners() const {
		return m_listeners;
	}

	template<class T> T* getListener() const;

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

	Entity* findChild(CString name);

	template <class FuncTy> void visitChildren(FuncTy boolFunc) {
		for (Entity* child : m_children) {
			if (boolFunc(child)) child->visitChildren(boolFunc);
		}
	}

protected:
	explicit Entity(const Entity* that);

	virtual void onCreate() {};
	virtual void onDestroy() {};
	virtual void onEnable() {};
	virtual void onDisable() {};
	virtual void onShow() {};
	virtual void onHide() {};
	virtual void onInvalidate() {};
	virtual void onValidate() {};
	virtual void onReset() {};

	virtual Entity* onCopy() const = 0;

private:
	friend class Scene;
	friend class EntityListener;

	enum struct Dirty { none = 0, localMatrix = 1, worldMatrix = 2 };

	mutable AffineMat4r m_worldMatrix;
	mutable AffineMat4r m_localMatrix;
	mutable Mat3r m_localBasis;
	mutable Vec3r m_localScale{1};
	mutable Dirty m_dirty{Dirty::none};

	Scene* m_scene{};
	bool m_isEnabled{true};
	bool m_isVisible{true};
	bool m_invalid{};

	String m_name;

	Entity* m_parent{};
	Vector<EntityPtr> m_children;

	Vector<EntityListenerPtr> m_listeners;

	void addListener(EntityListener* listener);

	void invalidateWorldMatrix();
	void invalidateLocalMatrix();

	void create(Scene* scene);
	void destroy();
	void enable();
	void disable();
	void show();
	void hide();
	void invalidate();
	void validate();
};

struct EntityListener : Shared {
	SGD_OBJECT_TYPE(EntityListener, Shared);

	Entity* entity() const {
		return m_entity;
	}

protected:
	friend class Entity;

	void attach(Entity* entity);

	virtual void onCreate() {
	}
	virtual void onDestroy() {
	}
	virtual void onEnable() {
	}
	virtual void onDisable() {
	}
	virtual void onShow() {
	}
	virtual void onHide() {
	}
	virtual void onInvalidate() {
	}
	virtual void onValidate() {
	}
	virtual void onReset() {
	}

private:
	Entity* m_entity{};
};

// ***** Inline *****

template<class T> T* Entity::getListener() const {
	for(EntityListener* l : m_listeners) if(l->is<T>()) return l->as<T>();
	return {};
}

} // namespace sgd
