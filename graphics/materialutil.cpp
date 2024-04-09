#include "materialutil.h"

#include "pbrmaterial.h"
#include "prelitmaterial.h"

#include "textureutil.h"

namespace sgd {

Material* createPBRMaterial(CVec4f albedoColor) {
	auto material = new Material(&pbrMaterialDescriptor);
	material->setVector4f("albedoColor4f", albedoColor);

	return material;
}

Expected<Material*, FileioEx> loadPBRMaterial(CPath path) {
	if (!path.isUrl() && !path.exists()) return FileioEx("Material directory does not exist");

	auto texFlags = TextureFlags::mipmap | TextureFlags::filter;

	Set<String> exts{".png",".jpg",".jpeg",".bmp",".tga",".gif"};
	if(exts.find(path.ext())!=exts.end()) {
		auto texture = loadTexture(path, TextureFormat::srgba8, texFlags);
		if(!texture) return texture.error();
		auto material = new Material(&pbrMaterialDescriptor);
		material->setTexture("albedoTexture", texture.result());
		return material;
	}

	auto tryLoad = [=](CString suffix, TextureFormat format) -> Texture* {
		auto texture = loadTexture(path / path.stem() + suffix, format, texFlags);
		return texture ? texture.result() : nullptr;
	};

	auto material = new Material(&pbrMaterialDescriptor);

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

Material* createPrelitMaterial(CVec4f albedoColor) {
	auto material = new Material(&prelitMaterialDescriptor);

	material->setVector4f("albedoColor4f", albedoColor);

	return material;
}

Expected<Material*, FileioEx> loadPrelitMaterial(CPath path) {
	auto texture = loadTexture(path, TextureFormat::srgba8, TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clampU | TextureFlags::clampV);
	if (!texture) return texture.error();

	auto material = createPrelitMaterial();
	material->setTexture("albedoTexture", texture.result());

	return material;
}

} // namespace sgd
