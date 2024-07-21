#include "uidevice.h"

#include "window.h"

#ifdef __EMSCRIPTEN__
#include "emscripten/html5.h"
#endif

namespace sgd {

// ***** UIDevice *****

UIDevice::UIDevice(uint32_t maxButtons) : m_maxButtons(maxButtons), m_buttons(new Button[maxButtons]) {
	beginPollEvents.connect(this, [=] { //
		resetKeyHits();
	});
}

void UIDevice::setButtonDown(uint32_t index, bool down) {
	SGD_ASSERT(index < m_maxButtons);

	auto& b = m_buttons[index];
	if (b.m_down == down) return;

	b.m_down = down;
	if (!down) return;

	b.m_hit = true;
	m_hits.push_back(index);
	b.pressed.emit();
}

void UIDevice::resetKeyHits() {
	for (auto index : m_hits) m_buttons[index].m_hit = false;
	m_hits.clear();
}

void UIDevice::flush() {
	for (auto b = m_buttons; b < m_buttons + m_maxButtons; ++b) {
		b->m_down = b->m_hit = false;
	}
	m_hits.clear();
}

} // namespace sgd
