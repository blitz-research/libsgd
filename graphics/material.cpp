#include "material.h"

#include "textureutil.h"

namespace sgd {

MaterialDescriptor::MaterialDescriptor(String typeName,								   //
									   const BindGroupDescriptor* bindGroupDescriptor, //
									   uint32_t uniformsSize,						   //
									   Map<String, UniformDesc> uniformDescs,		   //
									   Map<String, TextureDesc> textureDescs)
	: typeName(std::move(typeName)),			//
	  bindGroupDescriptor(bindGroupDescriptor), //
	  uniformsSize(uniformsSize),				//
	  uniformDescs(std::move(uniformDescs)),	//
	  textureDescs(std::move(textureDescs)) {
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

bool Material::setTexture(CString name, CTexture* texture) {
	auto it = m_desc->textureDescs.find(name);
	if (it == m_desc->textureDescs.end()) return false;

	if (name == "normalTexture") m_hasNormalTexture = texture;
	if (!texture) texture = it->second.defValue;

	m_bindGroup->setTexture(it->second.binding, texture);
	return true;
}

bool Material::setVector4f(CString name, CVec4f value) {
	SGD_ASSERT(endsWith(name, "4f"));

	auto it = m_desc->uniformDescs.find(name);
	if (it == m_desc->uniformDescs.end() || it->second.type != 4) return false;

	m_uniformBuffer->update(&value, it->second.offset, sizeof(value));
	return true;
}

bool Material::setVector3f(CString name, CVec3f value) {
	SGD_ASSERT(endsWith(name, "3f"));

	auto it = m_desc->uniformDescs.find(name);
	if (it == m_desc->uniformDescs.end() || it->second.type != 3) return false;

	m_uniformBuffer->update(&value, it->second.offset, sizeof(value));
	return true;
}

bool Material::setVector2f(CString name, CVec2f value) {
	SGD_ASSERT(endsWith(name, "2f"));

	auto it = m_desc->uniformDescs.find(name);
	if (it == m_desc->uniformDescs.end() || it->second.type != 2) return false;

	m_uniformBuffer->update(&value, it->second.offset, sizeof(value));
	return true;
}

bool Material::setFloat(CString name, float value) {
	SGD_ASSERT(endsWith(name, "1f"));

	auto it = m_desc->uniformDescs.find(name);
	if (it == m_desc->uniformDescs.end() || it->second.type != 1) return false;

	m_uniformBuffer->update(&value, it->second.offset, sizeof(value));
	return true;
}

bool Material::setInt(CString name, int value) {
	SGD_ASSERT(endsWith(name, "1i"));

	auto it = m_desc->uniformDescs.find(name);
	if (it == m_desc->uniformDescs.end() || it->second.type != 1) return false;

	m_uniformBuffer->update(&value, it->second.offset, sizeof(value));
	return true;
}

} // namespace sgd
