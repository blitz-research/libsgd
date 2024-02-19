#include "buffer.h"

namespace sgd {

Buffer::Buffer(BufferType type, const void* data, uint32_t size)
	: m_type(type), m_data((uint8_t*)std::malloc(size)), m_size(size), m_dirtyBegin(m_size), m_dirtyEnd(0) {

	if (data) {
		std::memcpy(m_data, data, size);
		std::swap(m_dirtyBegin, m_dirtyEnd);
	}
}

Buffer::~Buffer() {
	std::free(m_data);
}

void Buffer::resize(uint32_t size) {
	if (size == m_size) return;

	auto data = (uint8_t*)std::malloc(size);

	std::memcpy(data, m_data, std::min(size, m_size));

	std::swap(m_data, data);
	m_size = size;
	std::free(data);

	m_dirtyBegin = std::min(m_dirtyBegin, (uint32_t)m_size);
	m_dirtyEnd = std::min(m_dirtyEnd, m_size);

	m_wgpuBuffer = {};
	invalidate(true);
}

uint8_t* Buffer::lock(uint32_t offset, uint32_t size) {
	SGD_ASSERT(offset <= m_size && offset + size <= m_size);
	m_dirtyBegin = std::min(m_dirtyBegin, offset);
	m_dirtyEnd = std::max(m_dirtyEnd, offset + size);
	return m_data + offset;
}

void Buffer::unlock() {
	invalidate(false);
}

void Buffer::update(const void* data, uint32_t offset, uint32_t size) {
	std::memcpy(lock(offset, size), data, size);
	unlock();
}

void Buffer::onValidate(GraphicsContext* gc) const {

	if(!m_wgpuBuffer) {

		static const Map<BufferType, wgpu::BufferUsage> usages = {
			{BufferType::uniform, wgpu::BufferUsage::Uniform},	 {BufferType::storage, wgpu::BufferUsage::Storage},
			{BufferType::vertex, wgpu::BufferUsage::Vertex},	 {BufferType::index, wgpu::BufferUsage::Index},
			{BufferType::instance, wgpu::BufferUsage::Vertex},
			{BufferType::indirect, wgpu::BufferUsage::Indirect},
		};

		wgpu::BufferDescriptor desc{};
		desc.usage = usages.find(m_type)->second | wgpu::BufferUsage::CopyDst;
		desc.size = m_size;
		desc.mappedAtCreation = true;
		m_wgpuBuffer = gc->wgpuDevice().CreateBuffer(&desc);
		std::memcpy(m_wgpuBuffer.GetMappedRange(0, m_size), m_data, m_size);
		m_wgpuBuffer.Unmap();

		m_dirtyBegin = m_size;
		m_dirtyEnd = 0;
		return;
	}

	if (m_dirtyEnd > m_dirtyBegin) {
		SGD_ASSERT(m_dirtyBegin < m_size && m_dirtyEnd <= m_size);
		gc->wgpuDevice().GetQueue().WriteBuffer(m_wgpuBuffer, m_dirtyBegin, m_data + m_dirtyBegin, m_dirtyEnd - m_dirtyBegin);
	}

	m_dirtyBegin = m_size;
	m_dirtyEnd = 0;
}

} // namespace sgd
