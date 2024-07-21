#pragma once

#include "object.h"
#include "signal.h"	// NOLINT (header name)

#include <new>

namespace sgd {

// ***** Slot *****

template <class... ArgTys> Slot<ArgTys...>::Slot(const Signal<ArgTys...>* signal, Object* context) : context(context) {
	if (!context) return;
	deleter = context->deleted.connect(nullptr, [=] {
		this->deleter = nullptr;
		signal->disconnect(this);
	});
}

template <class... ArgTys> Slot<ArgTys...>::~Slot() {
	if (deleter) context->deleted.disconnect(deleter);
}

// ***** Signal *****

template <class... ArgTys>
template <class FunTy>
Signal<ArgTys...>::SlotImpl<FunTy>::SlotImpl(const Signal* signal, Object* context, const FunTy& fun)
	: SlotTy(signal, context), fun(fun) {
}

template <class... ArgTys> Signal<ArgTys...>::~Signal() {
	if (!m_slots) return;
	for (int i = 0; i < m_slots->size; ++i) {
		delete m_slots->slots[i];
	}
	std::free(m_slots);
}

template <class... ArgTys> template <class FunTy> void Signal<ArgTys...>::SlotImpl<FunTy>::invoke(ArgTys&&... args) {
	fun(std::forward<ArgTys>(args)...);
}

template <class... ArgTys>
template <class FunTy>
Slot<ArgTys...>* Signal<ArgTys...>::connect(Object* context, const FunTy& fun) const {
	auto slot = new SlotImpl<FunTy>(this, context, fun);
	if (m_slots) {
		for (int i = 0; i < m_slots->size; ++i) {
			if (m_slots->slots[i]) continue;
			m_slots->slots[i] = slot;
			return slot;
		}
		if (m_slots->size == m_slots->capacity) {
			auto capacity = m_slots->capacity * 2;
			auto buf = std::malloc(sizeof(SlotList) + sizeof(SlotTy*) * (capacity - 1));
			auto slots = new (buf) SlotList{m_slots->size, capacity};
			for (int i = 0; i < m_slots->size; ++i) slots->slots[i] = m_slots->slots[i];
			std::free(m_slots);
			m_slots = slots;
		}
	} else {
		static constexpr uint32_t capacity = 3; // initial capacity;
		auto buf = std::malloc(sizeof(SlotList) + sizeof(SlotTy*) * (capacity - 1));
		m_slots = new (buf) SlotList{0, capacity};
	}
	m_slots->slots[m_slots->size++] = slot;
	return slot;
}

template <class... ArgTys> bool Signal<ArgTys...>::connected(SlotTy* slot) const {
	if (!m_slots) return false;
	for (int i = 0; i < m_slots->size; ++i) {
		if (m_slots->slots[i] == slot) return true;
	}
	return false;
}

template <class... ArgTys> bool Signal<ArgTys...>::connected(Object* context) const {
	if (!m_slots) return false;
	for (int i = 0; i < m_slots->size; ++i) {
		auto slot = m_slots[i];
		if (slot && slot->context == context) return true;
	}
	return false;
}

template <class... ArgTys> void Signal<ArgTys...>::compact() const {

	// Trimming trailing empty slots should be safe...
	while (m_slots->size && !m_slots->slots[m_slots->size - 1]) --m_slots->size;

	// Might not be safe to change slot order while emit is happening?
	// while (m_slots->size && !m_slots->slots[0]) std::swap(m_slots->slots[0], m_slots->slots[--m_slots->size]);
}

template <class... ArgTys> void Signal<ArgTys...>::disconnect(SlotTy* slot) const {
	SGD_ASSERT(m_slots);
	for (int i = 0; i < m_slots->size; ++i) {
		if (m_slots->slots[i] != slot) continue;
		m_slots->slots[i] = nullptr;
		delete slot;
		compact();
		return;
	}
	SGD_ABORT();
}

template <class... ArgTys> void Signal<ArgTys...>::disconnect(Object* context) const {
	SGD_ASSERT(m_slots);
	for (int i = 0; i < m_slots->size; ++i) {
		auto slot = m_slots->slots[i];
		if (!slot || slot->context != context) continue;
		m_slots->slots[i] = nullptr;
		delete slot;
		compact();
		return;
	}
	SGD_ABORT();
}

template <class... ArgTys> void Signal<ArgTys...>::disconnectAll() const {
	if (!m_slots) return;
	for (int i = 0; i < m_slots->size; ++i) {
		delete m_slots->slots[i];
		m_slots->slots[i] = nullptr;
	}
	compact();
}

template <class... ArgTys> void Signal<ArgTys...>::emit(ArgTys... args) const {
	if (!m_slots) return;
	for (int i = 0, n = m_slots->size; i < n; ++i) {
		if (auto slot = m_slots->slots[i]) slot->invoke(std::forward<ArgTys>(args)...);
	}
}

} // namespace sgd
