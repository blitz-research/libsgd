#pragma once

#include "signal.h" // NOLINT (deprecated header)

//! @file

namespace sgd {

struct ObjectType {
	String name;
	ObjectType* const super;
	ObjectType* const succ;

	ObjectType(const char* name, ObjectType* super) noexcept : name(name), super(super), succ(g_allTypes) {
		g_allTypes = this;
	}

	constexpr bool extends(const ObjectType* that) const {
		for (auto type = this; type; type = type->super) {
			if (type == that) return true;
		}
		return false;
	}

	static inline ObjectType* allTypes() {
		return g_allTypes;
	}

	int instanceCount() const {
		return m_instanceCount;
	}

private:
	static inline ObjectType* g_allTypes;
	template <class T> friend struct InstanceCounter;

	int m_instanceCount{};
};

template <class T> struct InstanceCounter {
	InstanceCounter() {
		++T::staticType()->m_instanceCount;
	}
	~InstanceCounter() {
		--T::staticType()->m_instanceCount;
	}
};

#define SGD_INSTANCE_COUNTER(T) InstanceCounter<T> p_sgdInstanceCounter;

// clang-format off
#define SGD_OBJECT_TYPE(NAME, SUPER) \
	static inline ObjectType _sgdStaticType{#NAME, SUPER::staticType()}; \
	SGD_INSTANCE_COUNTER(NAME) \
	static inline constexpr ObjectType* staticType(){return &_sgdStaticType;} \
	ObjectType* dynamicType() const override {return &_sgdStaticType;}      \
	using Super = SUPER;
// clang-format on

struct Object;
using CObject = const Object;

struct Object {
	// clang-format off
	static inline ObjectType _sgdStaticType{"Object", nullptr};
	SGD_INSTANCE_COUNTER(Object)
	// clang-format on

	Object() = default;

	virtual ~Object() {
		deleted.emit();
	}

	Signal<> deleted;

	static inline constexpr ObjectType* staticType() {
		return &_sgdStaticType;
	}

	virtual ObjectType* dynamicType() const {
		return staticType();
	}

	bool is(ObjectType* type) const {
		return dynamicType()->extends(type);
	}

	template <class T> bool is() const {
		return dynamicType()->extends(T::staticType());
	}

	template <class T> T* as() {
		SGD_ASSERT(is<T>());
		return static_cast<T*>(this);
	}

	template <class T> const T* as() const {
		SGD_ASSERT(is<T>());
		return static_cast<const T*>(this);
	}
};

inline std::ostream& operator<<(std::ostream& os, const Object* object) {
	if (object) return os << object->dynamicType()->name << "@" << (const void*)object;
	return os << "<nullptr>";
}

void debugObjects(bool diffsOnly);

} // namespace sgd
