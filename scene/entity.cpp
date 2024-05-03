#include "entity.h"

#include "scene.h"

namespace sgd {

Entity::Entity() {
	init();
}

Entity::Entity(const Entity* that)
	: isEnabled(that->isEnabled()), //
	  isVisible(that->isVisible()) {
	init();
	for (Entity* child : that->m_children) child->copy()->setParent(this);
}

void Entity::setParent(Entity* parent) {

	if (m_parent) {
		SGD_ASSERT(sgd::contains(m_parent->m_children, this));
		sgd::rremove(m_parent->m_children, this);
	}

	if (parent) {
		m_parent = parent;
		SGD_ASSERT(!sgd::contains(m_parent->m_children, this));
		m_parent->m_children.emplace_back(this);
		invalidateWorldMatrix();
		invalidate();
		return;
	}

	auto matrix = worldMatrix();
	m_parent = nullptr;
	setWorldMatrix(matrix);
}

void Entity::setCollider(Collider* collider) {
	m_collider = collider;
}

void Entity::invalidate() {
	if(m_invalid || !m_scene) return;
	m_invalid = true;
	m_scene->m_invalid.push_back(this);
	invalidated.emit();
}

void Entity::invalidateWorldMatrix() { // NOLINT (recursive)
	if (bool(m_dirty & Dirty::worldMatrix)) return;
	m_dirty |= Dirty::worldMatrix;
	for (Entity* child : m_children) child->invalidateWorldMatrix();
	invalidate();
}

void Entity::invalidateLocalMatrix() { // NOLINT (recursive)
	m_dirty |= Dirty::localMatrix;
	invalidateWorldMatrix();
}

void Entity::setWorldMatrix(CAffineMat4r matrix) {
	setWorldPosition(matrix.t);
	setWorldBasis(normalize(matrix.r));
	setWorldScale(scale(matrix.r));
}

CAffineMat4r Entity::worldMatrix() const { // NOLINT (recursive)
	if (bool(m_dirty & Dirty::worldMatrix)) {
		m_worldMatrix = m_parent ? m_parent->worldMatrix() * localMatrix() : localMatrix();
		m_dirty &= ~Dirty::worldMatrix;
	}
	return m_worldMatrix;
}

void Entity::setLocalMatrix(CAffineMat4r matrix) {
	setLocalPosition(matrix.t);
	setLocalBasis(normalize(matrix.r));
	setLocalScale(scale(matrix.r));
}

CAffineMat4r Entity::localMatrix() const { // NOLINT (recursive)
	if (bool(m_dirty & Dirty::localMatrix)) {
		m_localMatrix.r = m_localBasis * Mat3r::scale(m_localScale);
		m_dirty &= ~Dirty::localMatrix;
	}
	return m_localMatrix;
}

void Entity::init() {
	isEnabled.changed.connect(nullptr, [=](bool e) {
		if (m_parent && !m_parent->enabled()) return;
		if (e) {
			onEnable();
			enabledChanged.emit(true);
			visitChildren([=](Entity* child) {
				if (!child->isEnabled()) return false;
				child->onEnable();
				enabledChanged.emit(true);
				return true;
			});
		} else {
			onDisable();
			enabledChanged.emit(false);
			visitChildren([=](Entity* child) {
				if (!child->isEnabled()) return false;
				child->onDisable();
				enabledChanged.emit(false);
				return true;
			});
		}
	});
	isVisible.changed.connect(nullptr, [=](bool v) {
		if (m_parent && !m_parent->visible()) return;
		if (v) {
			onShow();
			visibleChanged.emit(true);
			visitChildren([=](Entity* child) {
				if (!child->isVisible()) return false;
				child->onShow();
				visibleChanged.emit(true);
				return true;
			});
		} else {
			onHide();
			visibleChanged.emit(false);
			visitChildren([=](Entity* child) {
				if (!child->isVisible()) return false;
				child->onHide();
				visibleChanged.emit(false);
				return true;
			});
		}
	});
}

void Entity::create(Scene* scene) {
	m_scene = scene;
	onCreate();
	if(enabled()) onEnable();
	if(visible()) onShow();
}

void Entity::destroy() {
	isVisible = false;
	isEnabled = false;
	onDestroy();
	m_scene = nullptr;
}

void Entity::validate() {
	SGD_ASSERT(m_invalid);
	onValidate();
	m_invalid = false;
}

} // namespace sgd
