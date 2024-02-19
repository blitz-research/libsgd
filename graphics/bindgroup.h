#pragma once

#include "buffer.h"
#include "texture.h"

namespace sgd {

SGD_SHARED(BindGroup);

struct BindGroupDescriptor {

	uint32_t const wgpuBindGroupIndex;
	Vector<wgpu::BindGroupLayoutEntry> const wgpuBindGroupLayoutEntries;
	Vector<wgpu::VertexBufferLayout> const wgpuVertexBufferLayouts;
	String const wgpuShaderSource;
	uint32_t const hash;

	BindGroupDescriptor(uint32_t bindGroupIndex,									//
						CVector<wgpu::BindGroupLayoutEntry> bindGroupLayoutEntries, //
						CVector<wgpu::VertexBufferLayout> vertexBufferLayouts,		// Only for geometry shaders right now
						CString wgpuShaderSource);									// Shader source code for bind group

	wgpu::BindGroupLayout wgpuBindGroupLayout(GraphicsContext* gc) const;
};

struct BindGroup : GraphicsResource {
	SGD_OBJECT_TYPE(BindGroup, GraphicsResource);

	BindGroup(const BindGroupDescriptor* desc);

	const BindGroupDescriptor* descriptor() const {
		return m_desc;
	}

	void setBuffer(uint32_t index, Buffer* buffer);
	Buffer* getBuffer(uint32_t index) const;

	void setTexture(uint32_t index, Texture* texture);
	Texture* getTexture(uint32_t index) const;

	wgpu::BindGroup wgpuBindGroup() const {
		return m_bindGroup;
	}

private:
	const BindGroupDescriptor* m_desc;
	Vector<GraphicsResourcePtr> m_resources;

	mutable wgpu::BindGroup m_bindGroup;

	void setResource(uint32_t index, GraphicsResource* resource);

	void onValidate(GraphicsContext* gc) const override;
};

BindGroup* emptyBindGroup(uint32_t index);

} // namespace sgd
