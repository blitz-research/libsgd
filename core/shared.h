#pragma once

#include "log.h"
#include "object.h"
#include "std.h"

//! @file

//#define SGD_DEBUG_SHARED_REFS 1

namespace sgd {

struct Shared : public Object {
	SGD_OBJECT_TYPE(Shared, Object);

	void retain() const {
#if SGD_DEBUG_SHARED_REFS
		SGD_ASSERT(m_refs != deadrefs);
#else
		++m_refs;
#endif
	}

	void release() const {
#if SGD_DEBUG_SHARED_REFS
		SGD_ASSERT(m_refs && m_refs != deadrefs);
		if (!--m_refs) m_refs = deadrefs;
#else
		if (!--m_refs) delete this;
#endif
	}

	void validateRefs() const {
#if SGD_DEBUG_SHARED_REFS
		SGD_ASSERT(m_refs != deadrefs);
#endif
	}

protected:
	Shared() = default;
	~Shared() = default;

private:
	static constexpr auto deadrefs = 123456789u;

	mutable uint32_t m_refs = 0;
};

} // namespace sgd
