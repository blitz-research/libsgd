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

	Property<BlendMode> blendMode{BlendMode::opaque};

	Property<DepthFunc> depthFunc{DepthFunc::lessEqual};

	Property<CullMode> cullMode{CullMode::back};

	bool setVector4f(CString name, CVec4f value);

	bool setVector3f(CString name, CVec3f value);

	bool setVector2f(CString name, CVec2f value);

	bool setFloat(CString name, float value);

	bool setTexture(CString name, Texture* value);

	BindGroup* bindGroup() const {
		return m_bindGroup;
	}

protected:
	const MaterialDescriptor* m_desc;
	BufferPtr m_uniformBuffer;
	BindGroupPtr m_bindGroup;
};

} // namespace sgd
