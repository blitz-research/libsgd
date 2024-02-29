#include "material.h"

namespace sgd {

namespace {

Map<String, MaterialDescriptor*>* g_materialDescs;

}

MaterialDescriptor::MaterialDescriptor(CString typeName,							   //
									   const BindGroupDescriptor* bindGroupDescriptor, //
									   uint32_t uniformsSize,						   //
									   CMap<String, size_t> uniformOffsets,			   //
									   CMap<String, uint32_t> textureIndices,		   //
									   Function<void(Material*)> initFunc)
	: typeName(typeName),						//
	  bindGroupDescriptor(bindGroupDescriptor), //
	  uniformsSize(uniformsSize),				//
	  uniformOffsets(uniformOffsets),			//
	  textureIndices(textureIndices),			//
	  initFunc(initFunc) {
//	log() << "### Material typeName:" << typeName;
	if(!g_materialDescs) g_materialDescs= new Map<String, MaterialDescriptor*>();
	(*g_materialDescs)[typeName] = this;
}

Material::Material(const MaterialDescriptor* desc)
	: m_desc(desc), //
	  m_bindGroup(new BindGroup(m_desc->bindGroupDescriptor)),
	  m_uniformBuffer(new Buffer(BufferType::uniform, nullptr, m_desc->uniformsSize)) {

	addDependency(m_bindGroup);

	m_bindGroup->setBuffer(0, m_uniformBuffer);
	m_desc->initFunc(this);
}

Material::Material(CString typeName) : Material((*g_materialDescs)[typeName]) {
}

void Material::setBlendMode(BlendMode blendMode) {
	if (blendMode == m_blendMode) return;
	m_blendMode = blendMode;
	invalidate(true);
}

void Material::setDepthFunc(DepthFunc depthFunc) {
	if (depthFunc == m_depthFunc) return;
	m_depthFunc = depthFunc;
	invalidate(true);
}

void Material::setCullMode(CullMode cullMode) {
	if (cullMode == m_cullMode) return;
	m_cullMode = cullMode;
	invalidate(true);
}

void Material::setTexture(CString name, Texture* texture) {
	auto it = m_desc->textureIndices.find(name);
	SGD_ASSERT(it != m_desc->textureIndices.end());

	m_bindGroup->setTexture(it->second, texture);
}

void Material::setVector4f(CString name, CVec4f value) {
	SGD_ASSERT(endsWith(name, "4f"));
	auto it = m_desc->uniformOffsets.find(name);

	SGD_ASSERT(it != m_desc->uniformOffsets.end());

	m_uniformBuffer->update(&value, it->second, sizeof(value));
}

void Material::setVector3f(CString name, CVec3f value) {
	SGD_ASSERT(endsWith(name, "3f"));
	auto it = m_desc->uniformOffsets.find(name);
	SGD_ASSERT(it != m_desc->uniformOffsets.end());

	m_uniformBuffer->update(&value, it->second, sizeof(value));
}

void Material::setVector2f(CString name, CVec2f value) {
	SGD_ASSERT(endsWith(name, "2f"));
	auto it = m_desc->uniformOffsets.find(name);
	SGD_ASSERT(it != m_desc->uniformOffsets.end());

	m_uniformBuffer->update(&value, it->second, sizeof(value));
}

void Material::setFloat(CString name, float value) {
	SGD_ASSERT(endsWith(name, "1f"));
	auto it = m_desc->uniformOffsets.find(name);
	SGD_ASSERT(it != m_desc->uniformOffsets.end());

	m_uniformBuffer->update(&value, it->second, sizeof(value));
}

} // namespace sgd
