#include "uidevice.h"

#ifdef __EMSCRIPTEN__
#include "emscripten/html5.h"
#endif

namespace sgd {

// ***** UIDevice *****

UIDevice::UIDevice(uint32_t maxButtons) : m_maxButtons(maxButtons), m_buttons(new Button[maxButtons]) {
}

void UIDevice::setButtonState(uint32_t index, float value, bool down) {
	SGD_ASSERT(index < m_maxButtons);

	auto& b = m_buttons[index];

	b.m_value = value;
	b.m_down = down;
	if (!down) return;

	b.m_hit = true;
	m_hits.push_back(index);
	b.pressed.emit();
}

void UIDevice::setButtonValue(uint32_t index, float value) {
	SGD_ASSERT(index < m_maxButtons);

	auto& b = m_buttons[index];
	if (b.m_value == value) return;

	setButtonState(index, value, value > buttonDownThreshold);

	b.valueChanged.emit(value);
}

void UIDevice::setButtonDown(uint32_t index, bool down) {
	SGD_ASSERT(index < m_maxButtons);

	auto& b = m_buttons[index];
	if (b.m_down == down) return;

	setButtonState(index, float(down), down);
}

void UIDevice::flush() {
	for (auto b = m_buttons; b < m_buttons + m_maxButtons; ++b) {
		b->m_down = b->m_hit = false;
	}
	m_hits.clear();
}

void UIDevice::resetButtonHits() {
	for (auto index : m_hits) m_buttons[index].m_hit = false;
	m_hits.clear();
}

void UIDevice::update() {
	onPoll();
}

} // namespace sgd
