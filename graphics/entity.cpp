#include "entity.h"

#include "scene.h"

namespace sgd {

void Entity::setWorldMatrix(CAffineMat4f matrix) {
	m_worldMatrix = matrix;
}

void Entity::setEnabled(bool enabled) {
	if (enabled == m_enabled) return;

	if ((m_enabled = enabled)) {
		onEnable();
	} else {
		onDisable();
	}
}

void Entity::setVisible(bool visible) {
	if (visible == m_visible) return;

	if ((m_visible = visible)) {
		onShow();
	} else {
		onHide();
	}
}

} // namespace sgd
