#pragma once

#include "../core/bindgroup.h"
#include "../render/renderpipeline.h"

namespace sgd {

SGD_SHARED(Material);

struct MaterialDescriptor {
	struct UniformDesc {
		size_t offset;
		size_t type;	// n floats for now
		void* defValue;
	};
	struct TextureDesc {
		uint32_t binding;
		const Texture* defValue;
	};
	String const typeName;
	const BindGroupDescriptor* bindGroupDescriptor;
	uint32_t const uniformsSize;
	Map<String, UniformDesc> const uniformDescs;
	Map<String, TextureDesc> const textureDescs;
	uint32_t const mainTexture;

	MaterialDescriptor(String typeName,							   //
					   const BindGroupDescriptor* bindGroupDescriptor, //
					   uint32_t uniformsSize,						   //
					   Map<String, UniformDesc> uniformDescs,			   //
					   Map<String, TextureDesc> textureDescs,
					   uint32_t mainTexture);
};
using CMaterialDescriptor = const MaterialDescriptor;

struct Material : GraphicsResource {
	SGD_OBJECT_TYPE(Material, GraphicsResource);

	explicit Material(const MaterialDescriptor* desc);

	Property<Path> path;

	Property<BlendMode> blendMode{BlendMode::opaque};

	Property<DepthFunc> depthFunc{DepthFunc::lessEqual};

	Property<CullMode> cullMode{CullMode::back};

	String typeName()const{
		return m_desc->typeName;
	}

	CTexture* mainTexture() const {
		return m_desc->mainTexture ? m_bindGroup->getTexture(m_desc->mainTexture) : nullptr;
	}

	bool hasNormalTexture() const {
		return m_hasNormalTexture;
	}

	void setTexture(CString name, CTexture* texture);
	void setColor(CString name, CVec4f color);
	void setFloat(CString name, float value);

	wgpu::BindGroup wgpuBindGroup() const {
		return m_bindGroup->wgpuBindGroup();
	}

	BindGroup* bindGroup() const {
		return m_bindGroup;
	}

protected:
	const MaterialDescriptor* m_desc;
	BindGroupPtr m_bindGroup;
	BufferPtr m_uniformBuffer;
	bool m_hasNormalTexture{};
};

} // namespace sgd
