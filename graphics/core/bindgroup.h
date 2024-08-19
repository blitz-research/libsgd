#pragma once

#include "buffer.h"
#include "texture.h"

namespace sgd {

SGD_SHARED(BindGroup);

enum struct BindGroupType { scene, material, geometry, renderer };
constexpr int numBindGroupTypes = 4;

struct BindGroupDescriptor {
	const char* const label;
	BindGroupType const bindGroupType;
	Vector<wgpu::BindGroupLayoutEntry> const wgpuBindGroupLayoutEntries;
	String const wgpuShaderSource;
	Vector<wgpu::VertexBufferLayout> const wgpuVertexBufferLayouts;
	wgpu::PrimitiveTopology const wgpuTopology;
	uint32_t const hash;

	BindGroupDescriptor(const char* label,														//
						BindGroupType bindGroupType,											//
						Vector<wgpu::BindGroupLayoutEntry> bindGroupLayoutEntries,				//
						String wgpuShaderSource = {},											//
						Vector<wgpu::VertexBufferLayout> vertexBufferLayouts = {},				//
						wgpu::PrimitiveTopology topology = wgpu::PrimitiveTopology::Undefined); //

	wgpu::BindGroupLayout wgpuBindGroupLayout(GraphicsContext* gc) const;
};
using CBindGroupDescriptor = const BindGroupDescriptor;

struct BindGroup : GraphicsResource {
	SGD_OBJECT_TYPE(BindGroup, GraphicsResource);

	explicit BindGroup(CBindGroupDescriptor* desc);

	CBindGroupDescriptor* descriptor() const {
		return m_desc;
	}

	void setBuffer(uint32_t index, CBuffer* buffer);
	CBuffer* getBuffer(uint32_t index) const;

	void setTexture(uint32_t index, CTexture* texture, bool setSampler = true);
	void setSampler(uint32_t index, CTexture* texture);
	CTexture* getTexture(uint32_t index) const;

	wgpu::BindGroup wgpuBindGroup() const {
		SGD_ASSERT(valid());
		return m_wgpuBindGroup;
	}

private:
	CBindGroupDescriptor* m_desc;
	Vector<CGraphicsResourcePtr> m_resources;

	mutable wgpu::BindGroup m_wgpuBindGroup;

	void setResource(uint32_t index, CGraphicsResource* resource);

	void onValidate() const override;
};

BindGroup* emptyBindGroup(BindGroupType type);

inline BindGroup* emptyGeometry() {
	static auto geometry = emptyBindGroup(BindGroupType::geometry);
	return geometry;
}

inline BindGroup* emptyRenderer() {
	static auto renderer = emptyBindGroup(BindGroupType::renderer);
	return renderer;
}

} // namespace sgd
