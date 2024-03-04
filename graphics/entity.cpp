#include "entity.h"

#include "scene.h"

namespace sgd {

Entity::Entity(const Entity* that)
	: m_enabled(that->m_enabled), //
	  m_visible(that->m_visible) {
	for (auto child : that->m_children) child->copy()->setParent(this);
}

void Entity::invalidateWorldMatrix() const {
	if (bool(m_dirty & Dirty::worldMatrix)) return;
	for (auto child : m_children) child->invalidateWorldMatrix();
	validateLocalMatrix();
	m_dirty |= Dirty::worldMatrix;
}

void Entity::invalidateLocalMatrix() const {
	if (bool(m_dirty & Dirty::localMatrix)) return;
	validateWorldMatrix();
	m_dirty |= Dirty::localMatrix;
}

void Entity::validateWorldMatrix() const {
	if (!bool(m_dirty & Dirty::worldMatrix)) return;
	m_worldMatrix = m_parent ? m_parent->worldMatrix() * m_localMatrix : m_localMatrix;
	m_dirty &= ~Dirty::worldMatrix;
}

void Entity::validateLocalMatrix() const {
	if (!bool(m_dirty & Dirty::localMatrix)) return;
	m_localMatrix = m_parent ? inverse(m_parent->worldMatrix()) * m_worldMatrix : m_worldMatrix;
	m_dirty &= ~Dirty::localMatrix;
}

void Entity::setWorldMatrix(CAffineMat4f matrix) {
	for (auto child : m_children) child->invalidateWorldMatrix();
	m_worldMatrix = matrix;
	m_dirty = Dirty::localMatrix;
}

void Entity::setLocalMatrix(CAffineMat4f matrix) {
	for (auto child : m_children) child->invalidateWorldMatrix();
	m_localMatrix = matrix;
	m_dirty = Dirty::worldMatrix;
}

void Entity::setParent(Entity* parent) {
	if (parent) {
		invalidateWorldMatrix();
	} else {
		invalidateLocalMatrix();
	}
	if (m_parent) sgd::remove(m_parent->m_children, this);
	m_parent = parent;
	if (m_parent) m_parent->m_children.push_back(this);
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
