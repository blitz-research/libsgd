#include "materialutil.h"

#include "pbrmaterial.h"

#include "textureutil.h"

namespace sgd {

Expected<Material*, FileioEx> loadMaterial(CPath path) {

	if (!path.isUrl() && !path.isDir()) return FileioEx("Material directory does not exist");

	auto material = new Material(&pbrMaterialDescriptor);

	auto prefix = path / path.stem();

	auto texFlags = TextureFlags::mipmap | TextureFlags::filter;

	auto tryLoad = [=](CString suffix, TextureFormat format) -> Texture* {
		auto texture = loadTexture(prefix + suffix, format, texFlags);
		return texture ? texture.result() : nullptr;
	};

	if (auto texture = tryLoad("_Color.jpg", TextureFormat::srgba8)) {
		material->setTexture("albedoTexture", texture);
	}

	if (auto texture = tryLoad("_Metalness.jpg", TextureFormat::rgba8)) {
		material->setTexture("metallicTexture", texture);
		material->setFloat("metallicFactor1f", 1);
	}

	if (auto texture = tryLoad("_Roughness.jpg", TextureFormat::rgba8)) {
		material->setTexture("roughnessTexture", texture);
		material->setFloat("roughnessFactor1f", 1);
	}

	if (auto texture = tryLoad("_AmbientOcclusion.jpg", TextureFormat::rgba8)) {
		material->setTexture("occlusionTexture", texture);
	}

	if (auto texture = tryLoad("_NormalGL.jpg", TextureFormat::rgba8)) {
		material->setTexture("normalTexture", texture);
	}

	return material;
}

Material* rgbaMaterial(uint32_t rgba, float metallic, float roughness) {
	static Map<uint32_t, MaterialPtr> cache;

	auto& material = cache[rgba];
	if (material) return material;

	material = new Material(&pbrMaterialDescriptor);
	material->setVector4f("albedoColor4f", Vec4f::rgba(rgba));
	material->setFloat("metallicFactor1f", metallic);
	material->setFloat("roughnessFactor1f", roughness);
	return material;
}

Material* colorMaterial(CVec4f color, float metallic, float roughness) {
	return rgbaMaterial(rgba(color), metallic, roughness);
}

} // namespace sgd
