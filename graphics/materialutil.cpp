#include "materialutil.h"

#include "pbrmaterial.h"

#include "textureutil.h"

namespace sgd {

Expected<Material*, FileioEx> loadMaterial(CPath path) {

	if(!path.isDir()) return FileioEx("Material directory does not exist");

	auto material = new Material(&pbrMaterialDescriptor);

	auto prefix = path / path.stem();

	auto texFlags = TextureFlags::mipmap|TextureFlags::filter;

	if ((prefix + "_Color.jpg").exists()) {
		material->setTexture("albedoTexture",
							 loadTexture(prefix + "_Color.jpg", TextureFormat::srgba8, texFlags).result());
	}

	if ((prefix + "_Metalness.jpg").exists()) {
		material->setTexture("metallicTexture",
							 loadTexture(prefix + "_Metalness.jpg", TextureFormat::rgba8, texFlags).result());
		material->setFloat("metallicFactor1f", 1);
	}

	if ((prefix + "_Roughness.jpg").exists()) {
		material->setTexture("roughnessTexture",
							 loadTexture(prefix + "_Roughness.jpg", TextureFormat::rgba8, texFlags).result());
		material->setFloat("roughnessFactor1f", 1);
	}

	if ((prefix + "_AmbientOcclusion.jpg").exists()) {
		material->setTexture(
			"occlusionTexture",
			loadTexture(prefix + "_AmbientOcclusion.jpg", TextureFormat::rgba8, texFlags).result());
	}

	if ((prefix + "_NormalGL.jpg").exists()) {
		material->setTexture("normalTexture",
							 loadTexture(prefix + "_NormalGL.jpg", TextureFormat::rgba8, texFlags).result());
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
