#pragma once

#include "log.h"
#include "std.h"

//! @file

namespace sgd {

struct Object;

template <class... ArgTys> struct Signal;

template <class... ArgTys> struct Slot {
	using SignalTy = Signal<ArgTys...>;

private:
	friend SignalTy;

	Object* context;
	Slot<>* deleter{};

	Slot(const SignalTy* signal, Object* context);
	virtual ~Slot();

	virtual void invoke(ArgTys&&... args) = 0;

	void disconnect();
};

template <class... ArgTys> struct Signal {
public:
	using SlotTy = Slot<ArgTys...>;

	Signal() = default;
	~Signal();

	template <class FunTy> SlotTy* connect(Object* context, const FunTy& fun) const;

//	template <class ObjTy, class MemFunTy> SlotTy* connect(Object* context, const MemFunTy& memFun);

	bool connected(SlotTy* slot) const;
	bool connected(Object* context) const;

	void compact() const;

	void disconnect(SlotTy* slot) const;
	void disconnect(Object* context) const;

	void disconnectAll() const;

	void emit(ArgTys... args) const;

private:
	template <class FunTy> struct SlotImpl : public SlotTy {
		FunTy fun;

		SlotImpl(const Signal* signal, Object* context, const FunTy& fun);

		void invoke(ArgTys&&... args) override;
	};

	struct SlotList {
		uint32_t size;
		uint32_t capacity;
		SlotTy* emitting{};
		SlotTy* slots[1];
	};

	// Keep Signal small - one pointer!
	mutable SlotList* m_slots{};
};

}
