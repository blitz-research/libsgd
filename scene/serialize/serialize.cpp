#include "serialize.h"

#include "../model/gltfloader.h"
#include "../model/model.h"
#include "../overlay/overlay.h"
#include "../plane/planeentity.h"
#include "../scene/camera.h"
#include "../scene/light.h"
#include "../scene/scene.h"
#include "../scene/scenerenderer.h"
#include "../skybox/skybox.h"
#include "../sprite/sprite.h"

#include <json11.hpp>

using namespace json11;

namespace sgd {

namespace {

struct Deserializer {

	Rectf deserializeRectf(const Json& json) {
		auto& items = json.array_items();
		if (items.size() != 4) SGD_ERROR("OOPS");
		return Rectf((float)items[0].number_value(), (float)items[1].number_value(), (float)items[2].number_value(),
					 (float)items[3].number_value());
	}

	Vec4f deserializeVec4f(const Json& json) {
		auto& items = json.array_items();
		if (items.size() != 4) SGD_ERROR("OOPS");
		return Vec4f((float)items[0].number_value(), (float)items[1].number_value(), (float)items[2].number_value(),
					 (float)items[3].number_value());
	}

	Vec3f deserializeVec3f(const Json& json) {
		auto& items = json.array_items();
		if (items.size() != 3) SGD_ERROR("OOPS");
		return Vec3f((float)items[0].number_value(), (float)items[1].number_value(), (float)items[2].number_value());
	}

	template <class T> T* deserialize(const Json& json) {
		if (!json.is_number()) SGD_ERROR("OOPS");
		return m_assets[json.int_value()]->as<T>();
	}

	void deserializeEntity(const Json& json, Entity* entity) {

		if (!json["name"].is_null()) entity->setName(json["name"].string_value());

		if (!json["position"].is_null()) entity->setWorldPosition(deserializeVec3f(json["position"]));
		if (!json["rotation"].is_null()) entity->setWorldBasis(Mat3f::rotation(deserializeVec3f(json["rotation"])));
		if (!json["scale"].is_null()) entity->setWorldScale(deserializeVec3f(json["scale"]));

		if (!json["enabled"].is_null()) entity->setIsEnabled(json["enabled"].bool_value());
		if (!json["visible"].is_null()) entity->setIsVisible(json["visible"].bool_value());

		if (!json["children"].is_null()) {
			auto items = json["children"].array_items();
			for (Json& item : items) {
				auto child = deserializeObject(item);
				if (child) child->as<Entity>()->setParent(entity);
			}
		}
	}

	Camera* deserializeCamera(const Json& json) {

		auto type = (CameraType)json["type"].int_value();
		auto camera = new Camera(type);
		deserializeEntity(json, camera);

		if (!json["fov"].is_null()) camera->fov = (float)json["fov"].number_value();
		if (!json["near"].is_null()) camera->near = (float)json["near"].number_value();
		if (!json["far"].is_null()) camera->far = (float)json["far"].number_value();

		return camera;
	}

	Light* deserializeLight(const Json& json) {

		auto type = (LightType)json["type"].int_value();
		auto light = new Light(type);
		deserializeEntity(json, light);

		if (!json["color"].is_null()) light->color = deserializeVec4f(json["color"]);
		if (!json["range"].is_null()) light->range = (float)json["range"].number_value();
		if (!json["falloff"].is_null()) light->falloff = (float)json["falloff"].number_value();
		if (!json["innerConeAngle"].is_null()) light->innerConeAngle = (float)json["innerConeAngle"].number_value();
		if (!json["outerConeAngle"].is_null()) light->outerConeAngle = (float)json["innerConeAngle"].number_value();
		if (!json["shadowsEnabled"].is_null()) light->shadowsEnabled = json["shadowsEnabled"].bool_value();
		if (!json["priority"].is_null()) light->priority = json["priority"].int_value();

		return light;
	}

	Skybox* deserializeSkybox(const Json& json) {

		auto skybox = new Skybox();
		deserializeEntity(json, skybox);

		if (!json["skyTexture"].is_null()) skybox->skyTexture = deserialize<Texture>(json["skyTexture"]);
		if (!json["roughness"].is_null()) skybox->roughness = (float)json["roughness"].number_value();

		return skybox;
	}

	PlaneEntity* deserializePlane(const Json& json) {

		auto plane = new PlaneEntity();
		deserializeEntity(json, plane);

		if (!json["material"].is_null()) plane->bindings()->material = deserialize<Material>(json["material"]);
		if (!json["color"].is_null()) plane->bindings()->color = deserializeVec4f(json["color"]);

		return plane;
	}

	Model* deserializeModel(const Json& json) {

		auto model = new Model();
		deserializeEntity(json, model);

		if (!json["mesh"].is_null()) model->mesh = deserialize<Mesh>(json["mesh"]);
		if (!json["color"].is_null()) model->color = deserializeVec4f(json["color"]);

		return model;
	}

	Sprite* deserializeSprite(const Json& json) {

		auto sprite = new Sprite();
		deserializeEntity(json, sprite);

		if (!json["image"].is_null()) sprite->image = deserialize<Image>(json["image"]);
		sprite->color = deserializeVec4f(json["color"]);
		sprite->frame = (float)json["frame"].number_value();

		return sprite;
	}

	Scene* deserializeScene(const Json& json) {

		auto scene = new Scene();

		if (!json["assets"].is_null()) {
			auto& items = json["assets"].array_items();
			for (auto& item : items) {
				auto clas = item["class"].string_value();
				if (clas == "Mesh") {
					auto path = item["path"].string_value();
					auto mesh = loadStaticMesh(Path(path));
					m_assets.emplace_back(mesh.result());
				} else if (clas == "Material") {
					auto path = Path(item["path"].string_value());
					auto type = item["type"].string_value();
					Material* material;
					if (type == "pbr") {
						material = loadPBRMaterial(path).result();
					} else if (type == "prelit") {
						material = loadPrelitMaterial(path).result();
					} else {
						SGD_ERROR("OOPS");
					}
					m_assets.emplace_back(material);
				} else if (clas == "Texture") {
					auto path = Path(item["path"].string_value());
					auto type = (TextureType)item["type"].int_value();
					auto format = (TextureFormat)item["format"].int_value();
					auto flags = (TextureFlags)item["flags"].int_value();
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
					m_assets.emplace_back(texture);
				} else if (clas == "Image") {
					auto texture = deserialize<Texture>(item["texture"]);
					auto image = new Image(texture);
					image->viewMode = (ImageViewMode)(int)item["viewMode"].number_value();
					image->rect = deserializeRectf(item["rect"]);
					m_assets.emplace_back(image);
				} else {
					SGD_ERROR("OOPS");
				}
			}
		}

		if (!json["ambientLightColor"].is_null()) {
			scene->sceneRenderer()->ambientLightColor = deserializeVec4f(json["ambientLightColor"]);
		}
		if (!json["envTexture"].is_null()) {
			scene->sceneRenderer()->envTexture = deserialize<Texture>(json["envTexture"]);
		}
		if (!json["clearColor"].is_null()) {
			scene->sceneRenderer()->clearColor = deserializeVec4f(json["clearColor"]);
		}

		if (!json["entities"].is_null()) {
			auto& items = json["entities"].array_items();
			for (auto& item : items) {
				auto root = deserializeObject(item)->as<Entity>();
				scene->add(root);
			}
		}

		return scene;
	}

	Object* deserializeObject(const Json& json) {

		auto clas = json["class"].string_value();
		if (clas.empty()) SGD_ERROR("OOPS");

		if (clas == "Scene") return deserializeScene(json);
		if (clas == "Camera") return deserializeCamera(json);
		if (clas == "Light") return deserializeLight(json);
		if (clas == "Skybox") return deserializeSkybox(json);
		if (clas == "Plane") return deserializePlane(json);
		if (clas == "Model") return deserializeModel(json);
		if (clas == "Sprite") return deserializeSprite(json);

		return nullptr;

		SGD_ERROR("OOPS: " + clas);

		return nullptr;
	}

	Vector<SharedPtr<Object>> m_assets;
};

struct Serializer {

	Json serialize(CRectf value) {
		return Json::array{value.min.x, value.min.y, value.max.x, value.max.y};
	}

	Json serialize(CVec4f value) {
		return Json::array{value.x, value.y, value.z, value.w};
	}

	Json serialize(CVec3f value) {
		return Json::array{value.x, value.y, value.z};
	}

	template <class T> Json serialize(const T* obj, const Function<Json()>& jsonify) {
		if (!obj) return {};

		auto it = m_assetIds.find(obj);
		if (it != m_assetIds.end()) return it->second;

		auto json = jsonify();

		Json id((int)m_assets.size());
		m_assetIds.insert(std::make_pair(obj, id));
		m_assets.emplace_back(json);
		return id;
	}

	Json serialize(CTexture* texture) {
		return serialize(texture, [=]() -> Json {
			Json::object json;
			json["class"] = "Texture";
			json["path"] = texture->path().str();
			json["type"] = (int)texture->type();
			json["format"] = (int)texture->format();
			json["flags"] = (int)texture->flags();
			return json;
		});
	}

	Json serialize(CMaterial* material) {
		return serialize(material, [=]() -> Json {
			Json::object json;
			json["class"] = "Material";
			json["path"] = material->path().str();
			json["type"] = material->typeName();
			return json;
		});
	}

	Json serialize(CImage* image) {
		return serialize(image, [=]() -> Json {
			Json::object json;
			json["class"] = "Image";
			json["texture"] = serialize(image->texture());
			json["viewMode"] = (int)image->viewMode();
			json["rect"] = serialize(image->rect());
			return json;
		});
	}

	Json serialize(CMesh* mesh) {
		return serialize(mesh, [=]() -> Json {
			Json::object json;
			json["class"] = "Mesh";
			json["path"] = mesh->path().str();
			return json;
		});
	}

	Json::object serializeEntity(CEntity* entity) {
		Json::object json;

		if (!entity->name().empty()) json["name"] = entity->name();

		json["position"] = serialize(entity->worldPosition());
		json["rotation"] = serialize(rotation(entity->worldBasis()));
		json["scale"] = serialize(entity->worldScale());

		json["enabled"] = entity->isEnabled();
		json["visible"] = entity->isVisible();

		json11::Json::array children;
		for (Entity* child : entity->children()) {
			auto cjson = serializeObject(child);
			if (!cjson.is_null()) children.emplace_back(cjson);
		}
		if (!children.empty()) json["children"] = std::move(children);

		return json;
	}

	Json serializeCamera(CCamera* camera) {
		auto json = serializeEntity(camera);
		json["class"] = "Camera";
		json["type"] = (int)camera->type();
		json["fov"] = camera->fov();
		json["near"] = camera->near();
		json["far"] = camera->far();
		return json;
	}

	Json serializeLight(CLight* light) {
		auto json = serializeEntity(light);
		json["class"] = "Light";
		json["type"] = (int)light->type();
		json["color"] = serialize(light->color());
		json["range"] = light->range();
		json["falloff"] = light->falloff();
		json["innerConeAngle"] = light->innerConeAngle();
		json["outerConeAngle"] = light->outerConeAngle();
		json["shadowsEnabled"] = light->shadowsEnabled();
		json["priority"] = light->priority();
		return json;
	}

	Json serializeSkybox(CSkybox* skybox) {
		auto json = serializeEntity(skybox);
		json["class"] = "Skybox";
		json["skyTexture"] = serialize(skybox->skyTexture());
		json["roughness"] = skybox->roughness();
		return json;
	}

	Json serializePlane(CPlaneEntity* plane) {
		auto json = serializeEntity(plane);
		json["class"] = "Plane";
		json["material"] = serialize(plane->bindings()->material());
		json["color"] = serialize(plane->bindings()->color());
		return json;
	}

	Json serializeModel(CModel* model) {
		auto json = serializeEntity(model);
		json["class"] = "Model";
		json["mesh"] = serialize(model->mesh());
		json["color"] = serialize(model->color());
		return json;
	}

	Json serializeSprite(CSprite* sprite) {
		auto json = serializeEntity(sprite);
		json["class"] = "Sprite";
		json["image"] = serialize(sprite->image());
		json["color"] = serialize(sprite->color());
		json["frame"] = (float)sprite->frame();
		return json;
	}

	Json serializeScene(CScene* scene) {
		Json::object json;
		json["class"] = "Scene";
		json["ambientLightColor"] = serialize(scene->sceneRenderer()->ambientLightColor());
		json["envTexture"] = serialize(scene->sceneRenderer()->envTexture());
		json["clearColor"] = serialize(scene->sceneRenderer()->clearColor());
		Json::array entities;
		for (Entity* entity : scene->rootEntities()) {
			auto cjson = serializeObject(entity);
			if (cjson.is_null()) continue;
			entities.emplace_back(cjson);
		}
		json["entities"] = std::move(entities);
		json["assets"] = std::move(m_assets);
		return json;
	}

	Json serializeObject(CObject* obj) {
		if(!obj) return {};
		if (obj->is<Scene>()) return serializeScene(obj->as<Scene>());
		if (obj->is<Camera>()) return serializeCamera(obj->as<Camera>());
		if (obj->is<Light>()) return serializeLight(obj->as<Light>());
		if (obj->is<Skybox>()) return serializeSkybox(obj->as<Skybox>());
		if (obj->is<PlaneEntity>()) return serializePlane(obj->as<PlaneEntity>());
		if (obj->is<Model>()) return serializeModel(obj->as<Model>());
		if (obj->is<Sprite>()) return serializeSprite(obj->as<Sprite>());
		return {};
	}

	Map<CObject*, Json> m_assetIds;
	Json::array m_assets;
};

} // namespace

String serialize(Object* obj) {
	Serializer serializer{};
	auto json = serializer.serializeObject(obj);
	return json.dump();
}

Object* deserialize(CString jsrc) {
	String err;
	auto json = Json::parse(jsrc, err);
	if (!err.empty()) SGD_ERROR(err);
	Deserializer deserializer{};
	return deserializer.deserializeObject(json);
}

} // namespace sgd
