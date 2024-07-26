#pragma once

#include "graphics.h"

//! @file

namespace sgd {

//! BufferType enum
enum struct BufferType {
	uniform,
	storage,
	vertex,
	index,
};

SGD_SHARED(Buffer);

struct Buffer : GraphicsResource {
	SGD_OBJECT_TYPE(Buffer, GraphicsResource);

	Buffer(BufferType type, const void* data, uint32_t size);
	~Buffer();

	BufferType type() const {
		return m_type;
	}

	const void* data() const {
		return m_data;
	}

	uint32_t size() const {
		return m_size;
	}

	uint8_t* lock(uint32_t offset, uint32_t size) {
		SGD_ASSERT(offset <= m_size && offset + size <= m_size);
		m_dirtyBegin = std::min(m_dirtyBegin, offset);
		m_dirtyEnd = std::max(m_dirtyEnd, offset + size);
		return m_data + offset;
	}

	void unlock() {
		invalidate(false);
	}

	void update(const void* data, uint32_t offset, uint32_t size) {
		std::memcpy(lock(offset, size), data, size);
		unlock();
	}

	void resize(uint32_t size);

	const wgpu::Buffer& wgpuBuffer() const {
		SGD_ASSERT(valid());
		return m_wgpuBuffer;
	}

private:
	BufferType m_type;
	uint8_t* m_data;
	uint32_t m_size;
	uint32_t m_capacity;

	mutable uint32_t m_dirtyBegin;
	mutable uint32_t m_dirtyEnd;

	mutable wgpu::Buffer m_wgpuBuffer;
	mutable bool m_invalid{true};
	mutable uint32_t m_alloced{};

	void onValidate() const override;
};

} // namespace sgd
