#pragma once

#include "signal.h"

//! @file

namespace sgd {

struct ObjectType {
	String const name;
	ObjectType* const super;

	ObjectType(const char* name, ObjectType* super) noexcept : name(name), super(super) {
	}

	bool extends(const ObjectType* that) const {
		for (auto type = this; type; type = type->super) {
			if (type == that) return true;
		}
		return false;
	}
};

#define SGD_OBJECT_TYPE(name, super)                                                                                           \
	static inline ObjectType staticType{#name, &super::staticType};                                                            \
	ObjectType* dynamicType() const override { return &staticType; }                                                           \
	using Super = super;

struct Object {
	static inline ObjectType staticType{"Object", nullptr};

	Object() = default;

	virtual ~Object() {
		deleted.emit();
	}

	Signal<> deleted;

	virtual ObjectType* dynamicType() const {
		return &staticType;
	}

	bool is(ObjectType* type) const {
		return dynamicType()->extends(type);
	}

	template <class T> bool is() const {
		return dynamicType()->extends(&T::staticType);
	}

	template <class T> T* as() {
		SGD_ASSERT(is<T>());
		return static_cast<T*>(this);
	}

	template <class T> const T* as() const {
		SGD_ASSERT(is<T>());
		return static_cast<T*>(this);
	}

	template <class T> T* to() {
		return is<T>() ? static_cast<T*>(this) : nullptr;
	}

	template <class T> const T* to() const {
		return is<T>() ? static_cast<const T*>(this) : nullptr;
	}
};

inline std::ostream& operator<<(std::ostream& os, const Object* object) {
	os << object->dynamicType()->name << "@" << (const void*)object;
	return os;
}

} // namespace sgd
