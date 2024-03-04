#pragma once

#include "renderpipeline.h"

namespace sgd {

SGD_SHARED(Material);

struct MaterialDescriptor {
	String typeName;
	const BindGroupDescriptor* bindGroupDescriptor;
	uint32_t uniformsSize;
	Map<String, size_t> uniformOffsets;
	Map<String, uint32_t> textureIndices;
	Function<void(Material*)> initFunc;

	MaterialDescriptor(CString typeName,							   //
					   const BindGroupDescriptor* bindGroupDescriptor, //
					   uint32_t uniformsSize,						   //
					   CMap<String, size_t> uniformOffsets,			   //
					   CMap<String, uint32_t> textureIndices,		   //
					   Function<void(Material*)> initFunc);
};

struct Material : GraphicsResource {
	SGD_OBJECT_TYPE(Material, GraphicsResource);

	Material(const MaterialDescriptor* desc);

	Material(CString typeName);

	void setBlendMode(BlendMode blendMode);

	BlendMode blendMode() const {
		return m_blendMode;
	}

	void setDepthFunc(DepthFunc depthFunc);

	DepthFunc depthFunc() const {
		return m_depthFunc;
	}

	void setCullMode(CullMode cullMode);

	CullMode cullMode() const {
		return m_cullMode;
	}

	void setVector4f(CString name, CVec4f value);

	void setVector3f(CString name, CVec3f value);

	void setVector2f(CString name, CVec2f value);

	void setFloat(CString name, float value);

	void setTexture(CString name, Texture* value);

	BindGroup* bindGroup() const {
		return m_bindGroup;
	}

protected:
	const MaterialDescriptor* m_desc;
	BindGroupPtr m_bindGroup;
	BufferPtr m_uniformBuffer;

	BlendMode m_blendMode{BlendMode::opaque};
	DepthFunc m_depthFunc{DepthFunc::lessEqual};
	CullMode m_cullMode{CullMode::back};
};

} // namespace sgd
