#include "entity.h"

#include "scene.h"

namespace sgd {

Entity::Entity(const Entity* that)
	: m_isEnabled(that->m_isEnabled), //
	  m_isVisible(that->m_isVisible) {
	for (Entity* child : that->m_children) child->copy()->setParent(this);
}

void Entity::create(Scene* scene) {
	m_scene=scene;
	onCreate();
	for (EntityListener* listener : m_listeners) listener->onCreate(this);
	if(enabled()) enable();
	if(visible()) show();
}

void Entity::destroy() {
	setIsVisible(false);
	setIsEnabled(false);
	for(EntityListener* listener : m_listeners) listener->onDestroy(this);
	onDestroy();
}

void Entity::enable() {
	onEnable();
	for (EntityListener* listener : m_listeners) listener->onEnable(this);
}

void Entity::disable() {
	for (EntityListener* listener : m_listeners) listener->onDisable(this);
	onDisable();
}

void Entity::show() {
	onShow();
	for (EntityListener* listener : m_listeners) listener->onShow(this);
}

void Entity::hide() {
	for (EntityListener* listener : m_listeners) listener->onHide(this);
	onHide();
}

void Entity::invalidate() {
	if (m_invalid || !m_scene) return;
	m_invalid = true;
	m_scene->m_invalid.push_back(this);
	onInvalidate();
	for(EntityListener* listener : m_listeners) listener->onInvalidate(this);
}

void Entity::validate() {
	SGD_ASSERT(m_invalid);
	m_invalid = false;
	for(EntityListener* listener : m_listeners) listener->onValidate(this);
	onValidate();
}

void Entity::reset() {
	onReset();
	for(EntityListener* listener : m_listeners) listener->onReset(this);
}

void Entity::setIsEnabled(bool isEnabled) {
	if (isEnabled == m_isEnabled) return;
	m_isEnabled = isEnabled;
	if (!m_scene || (m_parent && !m_parent->enabled())) return;
	if (isEnabled) {
		enable();
		visitChildren([=](Entity* child) {
			if (!child->isEnabled()) return false;
			child->enable();
			return true;
		});
	} else {
		disable();
		visitChildren([=](Entity* child) {
			if (!child->isEnabled()) return false;
			child->disable();
			return true;
		});
	}
}

void Entity::setIsVisible(bool isVisible) {
	if (isVisible == m_isVisible) return;
	m_isVisible = isVisible;
	if (!m_scene || (m_parent && !m_parent->visible())) return;
	if(isVisible) {
		show();
		visitChildren([=](Entity* child) {
			if (!child->isVisible()) return false;
			child->show();
			return true;
		});
	} else {
		hide();
		visitChildren([=](Entity* child) {
			if (!child->isVisible()) return false;
			child->hide();
			return true;
		});
	}
}

// TODO: This probably has issues with visbile/enabled state.
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

void Entity::setName(CString name) {
	m_name=name;
}

void Entity::addListener(EntityListener* listener) {
	m_listeners.emplace_back(listener);
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

} // namespace sgd
