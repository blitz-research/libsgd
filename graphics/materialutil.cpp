#include "materialutil.h"

#include "mattematerial.h"
#include "pbrmaterial.h"
#include "spritematerial.h"

#include "textureutil.h"

namespace sgd {

Material* createPBRMaterial() {
	return new Material(&pbrMaterialDescriptor);
}

Expected<Material*, FileioEx> loadPBRMaterial(CPath path) {
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

Material* createMatteMaterial() {
	return new Material(&matteMaterialDescriptor);
}

Expected<Material*, FileioEx> loadMatteMaterial(CPath path) {
	auto texture = loadTexture(path, TextureFormat::srgba8, TextureFlags::mipmap | TextureFlags::filter);
	if (!texture) return texture.error();

	auto material = createMatteMaterial();
	material->setTexture("albedoTexture", texture.result());

	return material;
}

Material* createSpriteMaterial() {
	return new Material(&spriteMaterialDescriptor);
}

Expected<Material*, FileioEx> loadSpriteMaterial(CPath path) {
	auto texture = loadTexture(path, TextureFormat::srgba8, TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clampU | TextureFlags::clampV);
	if (!texture) return texture.error();

	auto material = createSpriteMaterial();
	material->setTexture("albedoTexture", texture.result());

	return material;
}

} // namespace sgd
