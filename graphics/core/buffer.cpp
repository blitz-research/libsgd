#include "buffer.h"

namespace sgd {

namespace {

const Map<BufferType, const char*> usageNames{
	{BufferType::uniform, "Uniform buffer"}, //
	{BufferType::storage, "Storage buffer"}, //
	{BufferType::vertex, "Vertex buffer"},	 //
	{BufferType::index, "Index buffer"},	 //
};

}

Buffer::Buffer(BufferType type, const void* data, uint32_t size)
	: m_type(type), m_data((uint8_t*)std::malloc(size)), m_size(size), m_capacity(m_size) {

	if (data) {
		std::memcpy(m_data, data, size);
		m_dirtyBegin = 0;
		m_dirtyEnd = size;
	} else {
		m_dirtyBegin = size;
		m_dirtyEnd = 0;
	}
}

Buffer::~Buffer() {
	if(currentGC()) currentGC()->wgpuFree(m_alloced, "Buffer");
	std::free(m_data);
}

void Buffer::resize(uint32_t size) {
	if (size == m_size) return;

	if (size > m_capacity) {
		m_capacity = std::max(std::max(m_capacity * 3 / 2, size), 16u);
		auto data = (uint8_t*)std::malloc(m_capacity);
		std::memcpy(data, m_data, m_size);
		std::swap(m_data, data);
		std::free(data);
		m_size = size;
		m_invalid = true;
		invalidate(true);
	} else {
		m_size = size;
		m_dirtyBegin = std::min(m_dirtyBegin, m_size);
		m_dirtyEnd = std::min(m_dirtyEnd, m_size);
		invalidate();
	}
}

void Buffer::onValidate() const {

	auto gc = currentGC();

	if (m_invalid) {
		static const Map<BufferType, wgpu::BufferUsage> usages{
			{BufferType::uniform, wgpu::BufferUsage::Uniform}, //
			{BufferType::storage, wgpu::BufferUsage::Storage}, //
			{BufferType::vertex, wgpu::BufferUsage::Vertex},   //
			{BufferType::index, wgpu::BufferUsage::Index},	   //
		};

		wgpu::BufferDescriptor desc{};
		desc.usage = usages.find(m_type)->second | wgpu::BufferUsage::CopyDst;
		desc.size = m_capacity + 3u & ~3u;
		desc.mappedAtCreation = true;

		gc->wgpuAllocing(desc.size, usageNames.find(m_type)->second);
		m_wgpuBuffer = gc->wgpuDevice().CreateBuffer(&desc);
		gc->wgpuFree(m_alloced, usageNames.find(m_type)->second);
		m_alloced = desc.size;

		std::memcpy(m_wgpuBuffer.GetMappedRange(0, m_size), m_data, m_size);
		m_wgpuBuffer.Unmap();
		m_invalid = false;

	} else if (m_dirtyEnd > m_dirtyBegin) {

		SGD_ASSERT(m_dirtyBegin < m_size && m_dirtyEnd <= m_size);
		gc->wgpuDevice().GetQueue().WriteBuffer(m_wgpuBuffer, m_dirtyBegin, m_data + m_dirtyBegin, m_dirtyEnd - m_dirtyBegin);
	}

	m_dirtyBegin = m_size;
	m_dirtyEnd = 0;
}

} // namespace sgd
