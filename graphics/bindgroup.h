#pragma once

#include "buffer.h"
#include "texture.h"

namespace sgd {

SGD_SHARED(BindGroup);

enum struct BindGroupType { scene, material, renderer };

struct BindGroupDescriptor {
	const char* const label;
	BindGroupType const bindGroupType;
	Vector<wgpu::BindGroupLayoutEntry> const wgpuBindGroupLayoutEntries;
	Vector<wgpu::VertexBufferLayout> const wgpuVertexBufferLayouts;
	String const wgpuShaderSource;
	uint32_t const hash;

	BindGroupDescriptor(const char* label,										   //
						BindGroupType bindGroupType,							   //
						Vector<wgpu::BindGroupLayoutEntry> bindGroupLayoutEntries, //
						Vector<wgpu::VertexBufferLayout> vertexBufferLayouts,	   //
						String wgpuShaderSource);								   //

	wgpu::BindGroupLayout wgpuBindGroupLayout(GraphicsContext* gc) const;
};

struct BindGroup : GraphicsResource {
	SGD_OBJECT_TYPE(BindGroup, GraphicsResource);

	explicit BindGroup(const BindGroupDescriptor* desc);

	const BindGroupDescriptor* descriptor() const {
		return m_desc;
	}

	void setBuffer(uint32_t index, CBuffer* buffer);
	CBuffer* getBuffer(uint32_t index) const;

	void setTexture(uint32_t index, CTexture* texture);
	CTexture* getTexture(uint32_t index) const;

	wgpu::BindGroup wgpuBindGroup() const {
		SGD_ASSERT(valid());
		return m_bindGroup;
	}

private:
	const BindGroupDescriptor* m_desc;
	Vector<CGraphicsResourcePtr> m_resources;

	mutable wgpu::BindGroup m_bindGroup;

	void setResource(uint32_t index, CGraphicsResource* resource);

	void onValidate(GraphicsContext* gc) const override;
};

BindGroup* emptyBindGroup(uint32_t index);

} // namespace sgd
