#include "entity.h"

#include "scene.h"

namespace sgd {

Entity::Entity(const Entity* that)
	: m_enabled(that->m_enabled), //
	  m_visible(that->m_visible) {
	for (Entity* child : that->m_children) child->copy()->setParent(this);
}

void Entity::invalidateWorldMatrix() const { // NOLINT (recursive)
	if (bool(m_dirty & Dirty::worldMatrix)) return;
	m_dirty |= Dirty::worldMatrix;
	for (Entity* child : m_children) child->invalidateWorldMatrix();
}

void Entity::invalidateLocalMatrix() const { // NOLINT (recursive)
	m_dirty |= Dirty::localMatrix;
	invalidateWorldMatrix();
}

void Entity::setWorldMatrix(CAffineMat4f matrix) {
	setWorldPosition(matrix.t);
	setWorldBasis(normalize(matrix.r));
	setWorldScale(scale(matrix.r));
}

AffineMat4f Entity::worldMatrix() const { // NOLINT (recursive)
	if (bool(m_dirty & Dirty::worldMatrix)) {
		m_worldMatrix = m_parent ? m_parent->worldMatrix() * localMatrix() : localMatrix();
		m_dirty &= ~Dirty::worldMatrix;
	}
	return m_worldMatrix;
}

void Entity::setLocalMatrix(CAffineMat4f matrix) {
	setLocalPosition(matrix.t);
	setLocalBasis(normalize(matrix.r));
	setLocalScale(scale(matrix.r));
}

AffineMat4f Entity::localMatrix() const { // NOLINT (recursive)
	if (bool(m_dirty & Dirty::localMatrix)) {
		m_localMatrix.r = m_localBasis * Mat3f::scale(m_localScale);
		m_dirty &= ~Dirty::localMatrix;
	}
	return m_localMatrix;
}

void Entity::setParent(Entity* parent) {

	if (m_parent) {
		SGD_ASSERT(sgd::contains(m_parent->m_children, this));
		sgd::rremove(m_parent->m_children, this);
	}

	if(parent) {
		m_parent = parent;
		SGD_ASSERT(!sgd::contains(m_parent->m_children, this));
		m_parent->m_children.emplace_back(this);
		invalidateWorldMatrix();
		return;
	}

	auto matrix = worldMatrix();
	m_parent = nullptr;
	setWorldMatrix(matrix);
}

void Entity::setEnabled(bool enabled) {
	if (enabled == m_enabled) return;

	m_enabled = enabled;

	if (m_scene) m_enabled ? onEnable() : onDisable();
}

void Entity::setVisible(bool visible) {
	if (visible == m_visible) return;

	m_visible = visible;

	if (m_scene) m_visible ? onShow() : onHide();
}

} // namespace sgd
