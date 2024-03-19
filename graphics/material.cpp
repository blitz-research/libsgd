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

bool Material::setTexture(CString name, Texture* texture) {
	auto it = m_desc->textureIndices.find(name);
	if(it==m_desc->textureIndices.end()) return false;

	m_bindGroup->setTexture(it->second, texture);
	return true;
}

bool Material::setVector4f(CString name, CVec4f value) {
	auto it = m_desc->uniformOffsets.find(name);
	if(it==m_desc->uniformOffsets.end()) return false;

	m_uniformBuffer->update(&value, it->second, sizeof(value));
	return true;
}

bool Material::setVector3f(CString name, CVec3f value) {
	SGD_ASSERT(endsWith(name, "3f"));
	auto it = m_desc->uniformOffsets.find(name);
	if(it == m_desc->uniformOffsets.end()) return false;

	m_uniformBuffer->update(&value, it->second, sizeof(value));
	return true;
}

bool Material::setVector2f(CString name, CVec2f value) {
	auto it = m_desc->uniformOffsets.find(name);
	if(it == m_desc->uniformOffsets.end()) return false;

	m_uniformBuffer->update(&value, it->second, sizeof(value));
	return true;
}

bool Material::setFloat(CString name, float value) {
	auto it = m_desc->uniformOffsets.find(name);
	if(it == m_desc->uniformOffsets.end()) return false;

	m_uniformBuffer->update(&value, it->second, sizeof(value));
	return true;
}

} // namespace sgd
