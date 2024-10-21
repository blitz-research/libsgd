#include "materialutil.h"

#include "pbrmaterial.h"
#include "prelitmaterial.h"
#include "emissivematerial.h"

#include "../core/textureutil.h"

#include <json11.hpp>

namespace sgd {
/*
{
	"class" : "Material",
			  "type" : "pbr",
					   "blendMode" : 1,
					   "depthFunc" : 1,
					   "cullMode" : 0,
					   "textures"
		: {"albedo" : {"class" : "Texture", "path" : " ./blah.png", "type" : 1, "format" : 4, "flags" : 31}},
		  "colors" : {"albedo" : [ 1, 1, 1, 1 ]},
					 "floats" : {
	}
} // namespace sgd
 */

Material* deserializeMaterial(const json11::Json& json) {
	auto desc = MaterialDescriptor::forTypeName(json["type"].string_value());
	if(!desc) SGD_ERROR("Unrecognized material type");
	auto material = new Material(desc);
	if(!json["blendMode"].is_null()) material->blendMode = (BlendMode)json["blendMode"].int_value();
	if(!json["depthFunc"].is_null()) material->depthFunc = (DepthFunc)json["depthFunc"].int_value();
	if(!json["cullMode"].is_null()) material->cullMode = (CullMode)json["cullMode"].int_value();
	for(auto& kv : json["textures"].object_items()) {
		auto& items = kv.second;
		auto path = items["path"].string_value();
		auto type = !items["type"].is_null() ? (TextureType)items["type"].int_value() : TextureType::e2d;
		auto format = !items["type"].is_null() ? (TextureFormat)items["format"].int_value() : TextureFormat::any;
		auto flags = !items["flags"].is_null() ? (TextureFlags)items["flags"].int_value() : TextureFlags::default_;
		Texture* texture;
		switch (type) {
		case TextureType::e2d:
			texture = load2DTexture(Path(path), format, flags).result();
			break;
		case TextureType::cube:
			texture = loadCubeTexture(Path(path), format, flags).result();
			break;
		case TextureType::array:
			texture = loadArrayTexture(Path(path), format, flags).result();
			break;
		case TextureType::cubeArray:
			SGD_ERROR("OOPS");
		}
		material->setTexture(kv.first, texture);
	}
	for(auto& kv : json["colors"].object_items() ){
		auto& items = kv.second.array_items();
		material->setColor(kv.first, Vec4f((float)items[0].number_value(), (float)items[1].number_value(),
										   (float)items[2].number_value(), (float)items[3].number_value()));
	}
	for(auto& kv : json["floats"].object_items() ) {
		material->setFloat(kv.first, (float)kv.second.number_value());
	}
	return material;
}

Expected<Material*, FileioEx> loadMaterial(CPath path) {
	auto tpath = path;
	if(toLower(tpath.ext())!="sgd") tpath = tpath / "material.sgd";
	auto str = loadString(tpath);
	if(!str) return str.error();
	String err;
	auto json = json11::Json::parse(str.result(),err,json11::JsonParse::COMMENTS);
	if(json.is_null()) return SGD_PATHEX("JSON error parsing",tpath);
	auto material = deserializeMaterial(json);
	return material;
}

Material* createPBRMaterial() {
	return new Material(&pbrMaterialDescriptor);
}

Expected<Material*, FileioEx> loadPBRMaterial(CPath path) {
	if (!path.isUrl() && !path.exists()) return SGD_FILEIOEX("Material directory does not exist");

	auto texFlags = TextureFlags::mipmap | TextureFlags::filter;

	Set<String> exts{".png", ".jpg", ".jpeg", ".bmp", ".tga", ".gif", ".exr"};
	if (exts.find(path.ext()) != exts.end()) {
		auto texture = load2DTexture(path, TextureFormat::any, texFlags);
		if (!texture) return texture.error();
		SGD_LOG << "Texture" << path.str() << "loaded, format:" << (int)texture.result()->format();
		auto material = new Material(&pbrMaterialDescriptor);
		material->setTexture("albedo", texture.result());
		material->path = path;
		return material;
	}

	auto tryLoad = [=](CString suffix, TextureFormat format) -> Texture* {
		auto texture = load2DTexture(path / path.stem() + suffix, format, texFlags);
		return texture ? texture.result() : nullptr;
	};

	auto material = new Material(&pbrMaterialDescriptor);
	material->path = path;

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

Material* createPrelitMaterial() {
	return new Material(&prelitMaterialDescriptor);
}

Expected<Material*, FileioEx> loadPrelitMaterial(CPath path) {

	auto flags = TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clamp;

	auto texture = load2DTexture(path, TextureFormat::any, flags);
	if (!texture) return texture.error();

	auto material = createPrelitMaterial();
	material->setTexture("albedo", texture.result());

	return material;
}

Material* createEmissiveMaterial() {
	return new Material(&emissiveMaterialDescriptor);
}

Expected<Material*, FileioEx> loadEmissiveMaterial(CPath path) {

	auto flags = TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clamp;

	auto texture = load2DTexture(path, TextureFormat::any, flags);
	if (!texture) return texture.error();

	auto material = createPrelitMaterial();
	material->setTexture("emissive", texture.result());

	return material;
}

} // namespace sgd
