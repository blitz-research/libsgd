#pragma once

#include "log.h"
#include "object.h"
#include "std.h"

//! @file

//#define SGD_DEBUG_SHARED_REFS 1

namespace sgd {

struct Shared : public Object {
	SGD_OBJECT_TYPE(Shared, Object);

	void validateRefs() const {
#if SGD_DEBUG_SHARED_REFS
		SGD_ASSERT(m_refs != deadrefs);
#endif
	}

	void retain() const {
#if SGD_DEBUG_SHARED_REFS
		validateRefs();
		++m_refs;
#else
		++m_refs;
#endif
	}

	void release() const {
#if SGD_DEBUG_SHARED_REFS
		validateRefs();
		if (!--m_refs) {
			this->~Shared();
			m_refs = deadrefs;
		}
#else
		if (!--m_refs) {
			delete this;
		}
#endif
	}

	int refs() const {
		return m_refs;
	}

protected:
	Shared() = default;

	~Shared() override = default;

private:
	static constexpr uint32_t deadrefs = 0x81828384u;

	mutable uint32_t m_refs = 0;
};

} // namespace sgd
