#pragma once

#include "object.h"
#include "signal.h" // NOLINT (header name)

#include <new>

namespace sgd {

// ***** Slot *****

template <class... ArgTys> Slot<ArgTys...>::Slot(const Signal<ArgTys...>* signal, Object* context) : context(context) {
	if (!context) return;
	deleter = context->deleted.connect(nullptr, [=] {
		disconnect();
		signal->disconnect(this); // Could cause this to be deleted, so clear context first
	});
}

template <class... ArgTys> void Slot<ArgTys...>::disconnect() {
	if (!context) return;
	context->deleted.disconnect(deleter);
	context = nullptr;
}

template <class... ArgTys> Slot<ArgTys...>::~Slot() {
	// Must have been disconnected before deleted!
	SGD_ASSERT(!context);
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
		auto slot = m_slots->slots[i];
		if (!slot) continue;
		slot->disconnect();
		delete slot;
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
		compact();
		if (m_slots->size == m_slots->capacity) {
			auto capacity = m_slots->capacity * 3 / 2;
			auto buf = std::malloc(sizeof(SlotList) + sizeof(SlotTy*) * (capacity - 1));	// NOLINT
			auto slots = new (buf) SlotList{m_slots->size, capacity, m_slots->emitting};
			sgd::copy(slots->slots, m_slots->slots, slots->size);
			std::free(m_slots);
			m_slots = slots;
		}
	} else {
		if (!m_slots) {
			static constexpr uint32_t capacity = 3; // initial capacity;
			auto buf = std::malloc(sizeof(SlotList) + sizeof(SlotTy*) * (capacity - 1)); // NOLINT
			m_slots = new (buf) SlotList{0, capacity};
		}
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
	if (m_slots->emitting) return;

	int put = 0;
	for (int get = 0; get < m_slots->size; ++get) {
		if (auto slot = m_slots->slots[get]) m_slots->slots[put++] = slot;
	}
	m_slots->size = put;
}

template <class... ArgTys> void Signal<ArgTys...>::disconnect(SlotTy* slot) const {
	SGD_ASSERT(m_slots);
	for (int i = 0; i < m_slots->size; ++i) {

		if (m_slots->slots[i] != slot) continue;

		m_slots->slots[i] = nullptr;
		slot->disconnect();
		if (m_slots->emitting == slot) return;

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
		slot->disconnect();
		if (m_slots->emitting == slot) return;

		delete slot;
		compact();
		return;
	}
	SGD_ABORT();
}

template <class... ArgTys> void Signal<ArgTys...>::disconnectAll() const {
	if (!m_slots) return;
	for (int i = 0; i < m_slots->size; ++i) {

		auto slot = m_slots->slots[i];
		if (!slot) continue;

		m_slots->slots[i] = nullptr;
		slot->disconnect();
		if (m_slots->emitting == slot) continue;

		delete slot;
	}
	compact();
}

template <class... ArgTys> void Signal<ArgTys...>::emit(ArgTys... args) const {
	if (!m_slots) return;
	if (m_slots->emitting) {
#if SGD_CONFIG_DEBUG
		SGD_LOG << "Warning: Signal::emit() ignored while same signal emitting";
#endif
		return;
	}
	// Snapshot size so newly connected slots will NOT be emitted this time around.
	for (int i = 0, n = m_slots->size; i < n; ++i) {

		auto slot = m_slots->slots[i];
		if (!slot) continue;

		m_slots->emitting = slot;
		slot->invoke(std::forward<ArgTys>(args)...);
		if (m_slots->slots[i] == slot) continue;
		SGD_ASSERT(!m_slots->slots[i]);	// Must have been disconnected.

		// Note: we can't delete a slot while it's being emitted as this could destroy the slot's lamba context.
		// This is ultimately why there's an emitting field at all. Might be better/simpler to just queue these deletes?
		delete slot;
	}
	m_slots->emitting = nullptr;
	compact();
}

} // namespace sgd
