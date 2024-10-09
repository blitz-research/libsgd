#include "serialize.h"

#include "../model/gltfloader.h"
#include "../model/model.h"
#include "../overlay/overlay.h"
#include "../scene/camera.h"
#include "../scene/light.h"
#include "../scene/scene.h"
#include "../scene/scenerenderer.h"
#include "../skybox/skybox.h"

#include <json11.hpp>

using namespace json11;

namespace sgd {

namespace {

struct Deserializer {

	Vec4f deserializeVec4f(const Json& json) {
		auto& items = json.array_items();
		if (items.size() != 4) SGD_ERROR("OOPS");
		return Vec4f(items[0].number_value(), items[1].number_value(), items[2].number_value(), items[3].number_value());
	}

	Vec3f deserializeVec3f(const Json& json) {
		auto& items = json.array_items();
		if (items.size() != 3) SGD_ERROR("OOPS");
		return Vec3f(items[0].number_value(), items[1].number_value(), items[2].number_value());
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
				auto child = deserializeObject(item)->as<Entity>();
				child->setParent(entity);
			}
		}
	}

	Light* deserializeLight(const Json& json) {

		auto type = (LightType)json["type"].int_value();

		auto light = new Light(type);

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

	Model* deserializeModel(const Json& json) {

		auto model = new Model();

		deserializeEntity(json, model);

		if (!json["mesh"].is_null()) model->mesh = deserialize<Mesh>(json["mesh"]);
		if (!json["color"].is_null()) model->color = deserializeVec4f(json["color"]);

		return model;
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
				} else if (clas == "Texture") {
					auto path = item["path"].string_value();
					auto format = (TextureFormat)item["format"].int_value();
					auto flags = (TextureFlags)item["flags"].int_value();
					if (bool(flags & TextureFlags::array)) {
						auto texture = loadArrayTexture(Path(path), format, flags);
						m_assets.emplace_back(texture.result());
					} else if (bool(flags & TextureFlags::cube)) {
						auto texture = loadCubeTexture(Path(path), format, flags);
						m_assets.emplace_back(texture.result());
					} else {
						auto texture = load2DTexture(Path(path), format, flags);
						m_assets.emplace_back(texture.result());
					}
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

		if (!json["rootEntities"].is_null()) {
			auto& items = json["rootEntities"].array_items();
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
		if (clas == "Light") return deserializeLight(json);
		if (clas == "Model") return deserializeModel(json);
		if (clas == "Skybox") return deserializeSkybox(json);

		SGD_ERROR("OOPS: " + clas);

		return nullptr;
	}

	Vector<SharedPtr<Object>> m_assets;
};

struct Serializer {

	Json serialize(CVec4f value) {
		return Json::array{value.x, value.y, value.z, value.w};
	}

	Json serialize(CVec3f value) {
		return Json::array{value.x, value.y, value.z};
	}

	template <class T> Json serialize(const T* obj, const Function<Json()>& jsonify) {
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
			json["format"] = (int)texture->format();
			json["flags"] = (int)texture->flags();
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

	void serializeEntity(CEntity* entity, Json::object& json) {

		if (!entity->name().empty()) json["name"] = entity->name();

		json["position"] = serialize(entity->worldPosition());
		json["rotation"] = serialize(rotation(entity->worldBasis()));
		json["scale"] = serialize(entity->worldScale());

		json["enabled"] = entity->isEnabled();
		json["visible"] = entity->isVisible();

		if (!entity->children().empty()) {
			json11::Json::array children;
			for (auto& child : entity->children()) {
				children.emplace_back(serializeObject(child));
			}
			json["children"] = std::move(children);
		}
	}

	Json serializeLight(CLight* light) {
		Json::object json;
		json["class"] = "Light";

		serializeEntity(light, json);

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
		Json::object json;
		json["class"] = "Skybox";

		serializeEntity(skybox, json);

		if (skybox->skyTexture()) json["skyTexture"] = serialize(skybox->skyTexture());
		json["roughness"] = skybox->roughness();

		return json;
	}

	Json serializeModel(CModel* model) {

		Json::object json;
		json["class"] = "Model";

		serializeEntity(model, json);

		if (model->mesh()) json["mesh"] = serialize(model->mesh());
		json["color"] = serialize(model->color());

		return json;
	}

	Json serializeScene(CScene* scene) {

		Json::object json;
		json["class"] = "Scene";

		json["ambientLightColor"] = serialize(scene->sceneRenderer()->ambientLightColor());
		if (scene->sceneRenderer()->envTexture()) json["envTexture"] = serialize(scene->sceneRenderer()->envTexture());
		json["clearColor"] = serialize(scene->sceneRenderer()->clearColor());

		if (!scene->rootEntities().empty()) {
			Json::array roots;
			for (auto& root : scene->rootEntities()) {

				// Skip overlay for now.
				if (root->is<Overlay>()) continue;

				roots.emplace_back(serializeObject(root));
			}
			json["rootEntities"] = std::move(roots);
		}

		if (!m_assets.empty()) {
			json["assets"] = std::move(m_assets);
		}

		return json;
	}

	Json serializeObject(CObject* obj) {

		if (obj->is<Scene>()) return serializeScene(obj->as<Scene>());
		if (obj->is<Light>()) return serializeLight(obj->as<Light>());
		if (obj->is<Model>()) return serializeModel(obj->as<Model>());
		if (obj->is<Skybox>()) return serializeSkybox(obj->as<Skybox>());

		SGD_ERROR("OOPS:" + obj->dynamicType()->name);

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
