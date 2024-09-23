#include "material.h"

#include "../core/textureutil.h"

namespace sgd {

MaterialDescriptor::MaterialDescriptor(String typeName,								   //
									   const BindGroupDescriptor* bindGroupDescriptor, //
									   uint32_t uniformsSize,						   //
									   Map<String, UniformDesc> uniformDescs,		   //
									   Map<String, TextureDesc> textureDescs,
									   uint32_t mainTexture)
	: typeName(std::move(typeName)),			//
	  bindGroupDescriptor(bindGroupDescriptor), //
	  uniformsSize(uniformsSize),				//
	  uniformDescs(std::move(uniformDescs)),	//
	  textureDescs(std::move(textureDescs)), mainTexture(mainTexture) {
}

Material::Material(const MaterialDescriptor* desc)
	: m_desc(desc), //
	  m_bindGroup(new BindGroup(m_desc->bindGroupDescriptor)) {

	auto uniforms = (uint8_t*)std::malloc(m_desc->uniformsSize);
	for (auto& kv : m_desc->uniformDescs) {
		std::memcpy(uniforms + kv.second.offset, kv.second.defValue, kv.second.type * 4);
	}
	m_uniformBuffer = new Buffer(BufferType::uniform, uniforms, m_desc->uniformsSize);
	std::free(uniforms);

	m_bindGroup->setBuffer(0, m_uniformBuffer);

	for (auto& kv : m_desc->textureDescs) {
		m_bindGroup->setTexture(kv.second.binding, kv.second.defValue);
	}
	blendMode.changed.connect(this, [=](BlendMode) { //
		invalidate(true);
	});
	depthFunc.changed.connect(this, [=](DepthFunc) { //
		invalidate(true);
	});
	cullMode.changed.connect(this, [=](CullMode) { //
		invalidate(true);
	});
	addDependency(m_bindGroup);
}

void Material::setTexture(CString name, CTexture* texture) {

	auto it = m_desc->textureDescs.find(name);
	if (it == m_desc->textureDescs.end()) SGD_ERROR("Material texture paramter \"" + name + "\" not found");

	if (name == "normal") m_hasNormalTexture = bool(texture);
	if (!texture) texture = it->second.defValue;

	m_bindGroup->setTexture(it->second.binding, texture);
}

void Material::setColor(CString name, CVec4f color) {

	auto it = m_desc->uniformDescs.find(name);
	if (it == m_desc->uniformDescs.end()) SGD_ERROR("Material color parameter \"" + name + "\" not found");

	if (it->second.type != 4) SGD_ERROR("Material parameter \"" + name + "\" is not of type 'color'");

	auto lcolor = toLinearColor(color);

	m_uniformBuffer->update(&lcolor, it->second.offset, sizeof(lcolor));
}

void Material::setFloat(CString name, float value) {

	auto it = m_desc->uniformDescs.find(name);
	if (it == m_desc->uniformDescs.end()) SGD_ERROR("Material float parameter \"" + name + "\" not found");

	if (it->second.type != 1) SGD_ERROR("Material paramter \"" + name + "\" is not of type 'float'");

	m_uniformBuffer->update(&value, it->second.offset, sizeof(value));
}

} // namespace sgd
