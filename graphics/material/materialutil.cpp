#include "materialutil.h"

#include "pbrmaterial.h"
#include "prelitmaterial.h"

#include "../core/textureutil.h"

namespace sgd {

Material* createPBRMaterial(CVec4f albedoColor) {
	auto material = new Material(&pbrMaterialDescriptor);
	material->setColor("albedo", albedoColor);
	return material;
}

Expected<Material*, FileioEx> loadPBRMaterial(CPath path) {
	if (!path.isUrl() && !path.exists()) return SGD_FILEIOEX("Material directory does not exist");

	auto texFlags = TextureFlags::mipmap | TextureFlags::filter;

	Set<String> exts{".png", ".jpg", ".jpeg", ".bmp", ".tga", ".gif", ".exr"};
	if (exts.find(path.ext()) != exts.end()) {
		auto texture = load2DTexture(path, TextureFormat::any, texFlags);
		if (!texture) return texture.error();
		SGD_LOG << "Texture"<<path.str()<<"loaded, format:"<<(int)texture.result()->format();
		auto material = new Material(&pbrMaterialDescriptor);
		material->setTexture("albedo", texture.result());
		return material;
	}

	auto tryLoad = [=](CString suffix, TextureFormat format) -> Texture* {
		auto texture = load2DTexture(path / path.stem() + suffix, format, texFlags);
		return texture ? texture.result() : nullptr;
	};

	auto material = new Material(&pbrMaterialDescriptor);

	if (auto texture = tryLoad("_Color.jpg", TextureFormat::any)) {
		material->setTexture("albedo", texture);
	}

	if (auto texture = tryLoad("_Metalness.jpg", TextureFormat::rgba8)) {
		material->setTexture("metallic", texture);
		material->setFloat("metallic", 1);
	}

	if (auto texture = tryLoad("_Roughness.jpg", TextureFormat::rgba8)) {
		material->setTexture("roughness", texture);
		material->setFloat("roughness", 1);
	}

	if (auto texture = tryLoad("_AmbientOcclusion.jpg", TextureFormat::rgba8)) {
		material->setTexture("occlusion", texture);
	}

	if (auto texture = tryLoad("_NormalGL.jpg", TextureFormat::rgba8)) {
		material->setTexture("normal", texture);
	}

	return material;
}

Material* createPrelitMaterial(CVec4f albedoColor) {
	auto material = new Material(&prelitMaterialDescriptor);

	material->setColor("albedo", albedoColor);

	return material;
}

Expected<Material*, FileioEx> loadPrelitMaterial(CPath path) {

	auto flags = TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clamp;

	auto texture = load2DTexture(path, TextureFormat::any, flags);
	if (!texture) return texture.error();

	auto material = createPrelitMaterial();
	material->setTexture("albedo", texture.result());

	return material;
}

} // namespace sgd
