#pragma once

#include "graphics.h"

//! @file

namespace sgd {

//! BufferType enum
enum struct BufferType {
	vertex,
	index,
	instance,
	uniform,
	storage,
	indirect,
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

	void resize(uint32_t size);

	uint8_t* lock(uint32_t offset, uint32_t size);

	void unlock();

	void update(const void* data, uint32_t offset, uint32_t size);

	const wgpu::Buffer& wgpuBuffer() const {
		return m_wgpuBuffer;
	}

private:
	BufferType m_type;
	uint8_t* m_data;
	uint32_t m_size;

	mutable uint32_t m_dirtyBegin;
	mutable uint32_t m_dirtyEnd;

	mutable wgpu::Buffer m_wgpuBuffer;

	void onValidate(GraphicsContext* gc) const override;
};

} // namespace sgd
