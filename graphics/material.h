#pragma once

#include "renderpipeline.h"

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
	String typeName;
	const BindGroupDescriptor* bindGroupDescriptor;
	uint32_t uniformsSize;
	Map<String, UniformDesc> uniformDescs;
	Map<String, TextureDesc> textureDescs;

	MaterialDescriptor(String typeName,							   //
					   const BindGroupDescriptor* bindGroupDescriptor, //
					   uint32_t uniformsSize,						   //
					   Map<String, UniformDesc> uniformDescs,			   //
					   Map<String, TextureDesc> textureDescs);
};

struct Material : GraphicsResource {
	SGD_OBJECT_TYPE(Material, GraphicsResource);

	explicit Material(const MaterialDescriptor* desc);

	Property<BlendMode> blendMode{BlendMode::opaque};

	Property<DepthFunc> depthFunc{DepthFunc::lessEqual};

	Property<CullMode> cullMode{CullMode::back};

	bool setVector4f(CString name, CVec4f value);

	bool setVector3f(CString name, CVec3f value);

	bool setVector2f(CString name, CVec2f value);

	bool setFloat(CString name, float value);

	bool setTexture(CString name, CTexture* value);

	bool hasNormalTexture() const {
		return m_hasNormalTexture;
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
