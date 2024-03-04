#include "gltfloader.h"

#include "animation.h"
#include "meshutil.h"
#include "pbrmaterial.h"
#include "textureutil.h"

#include <tiny_gltf.h>

namespace sgd {

namespace {

AffineMat4f nodeMatrix(const tinygltf::Node& gltfNode) {

	AffineMat4f matrix;

	if (gltfNode.matrix.size() == 16) {
		float r[16];
		for (int i = 0; i < 16; ++i) r[i] = (float)gltfNode.matrix[i];
		matrix.r.i = {r[0], r[1], -r[2]};
		matrix.r.j = {r[4], r[5], -r[6]};
		matrix.r.k = {-r[8], -r[9], r[10]};
		matrix.t = {r[12], r[13], -r[14]};
		return matrix;
	}

	if (gltfNode.translation.size() == 3) {
		matrix = AffineMat4f::translation(
			{(float)gltfNode.translation[0], (float)gltfNode.translation[1], -(float)gltfNode.translation[2]});
	}

	if (gltfNode.rotation.size() == 4) {
		Quatf quat({-(float)gltfNode.rotation[0], //
					-(float)gltfNode.rotation[1], //
					(float)gltfNode.rotation[2]}, //
				   -(float)gltfNode.rotation[3]);
		matrix *= AffineMat4f(Mat3f::rotation(quat), {});
	}

	if (gltfNode.scale.size() == 3) {
		matrix *= AffineMat4f::scale({(float)gltfNode.scale[0], (float)gltfNode.scale[1], (float)gltfNode.scale[2]});
	}

	return matrix;
}

struct GLTFLoader {

	tinygltf::Model gltfModel;
	tinygltf::Scene* defaultScene;

	Vector<bool> cachedImages;
	Vector<TexturePtr> cachedTextures;
	Vector<MaterialPtr> cachedMaterials;
	Vector<EntityPtr> modelBones;

	Vector<uint16_t> uint16Indices;
	Vector<uint8_t> uint8Indices;

	Vector<Vertex> vertices;
	Map<int, Vector<Triangle>> triangles; // keyed by material index
	bool hasNormals{};

	Expected<bool, FileioEx> load(CPath path) {

		auto data = loadData(path).result();

		std::string err;
		std::string warn;
		tinygltf::TinyGLTF gltfLoader;

		bool res;
		auto magic = "glTF";
		if (data.size() >= 4 && !std::memcmp(data.data(), magic, 4)) {
			res = gltfLoader.LoadBinaryFromMemory(&gltfModel, &err, &warn, data.data(), data.size());
		} else {
			auto baseDir = path.resolve().parent_path().u8string();
			res = gltfLoader.LoadASCIIFromString(&gltfModel, &err, &warn, (char*)data.data(), data.size(), baseDir);
		}
		if (!warn.empty()) log() << ">>> Tiny gltf warning: " << warn;

		if (!res && err.empty()) err = "Unknown error.";

		if (!err.empty()) return FileioEx("Tiny gltf error: " + err);

		if (gltfModel.defaultScene == -1) {
			return FileioEx("Gltf Model has no default scene");
		}
		defaultScene = &gltfModel.scenes[gltfModel.defaultScene];

		cachedImages.resize(gltfModel.images.size());
		cachedTextures.resize(gltfModel.textures.size());
		cachedMaterials.resize(gltfModel.materials.size());

		modelBones.resize(gltfModel.nodes.size());

		return true;
	}

	Texture* getTexture(int id) {
		if (cachedTextures[id]) return cachedTextures[id];

		auto& gltfTex = gltfModel.textures[id];

		auto& gltfImage = gltfModel.images[gltfTex.source];

		if (gltfImage.bits != 8) SGD_PANIC("gltfImage.bits: " + std::to_string(gltfImage.bits));
		if (gltfImage.component != 4) SGD_PANIC("gltfImage.component: " + std::to_string(gltfImage.component));

		auto texFormat = TextureFormat::srgba8;
		auto texFlags = TextureFlags::none;

		if (!cachedImages[gltfTex.source]) {
			premultiplyAlpha(gltfImage.image.data(), TextureFormat::srgba8, Vec2u(gltfImage.width, gltfImage.height),
							 gltfImage.width * bytesPerTexel(texFormat));
			cachedImages[gltfTex.source] = true;
		}

		if (gltfTex.sampler >= 0) {
			SGD_ASSERT(gltfTex.sampler < gltfModel.samplers.size());
			auto& gltfSampler = gltfModel.samplers[gltfTex.sampler];
			if (gltfSampler.wrapS == TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE) texFlags |= TextureFlags::clampU;
			if (gltfSampler.wrapT == TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE) texFlags |= TextureFlags::clampV;

			switch (gltfSampler.magFilter) {
			case TINYGLTF_TEXTURE_FILTER_LINEAR:
				texFlags |= TextureFlags::filter;
				break;
			default:;
			}
			switch (gltfSampler.minFilter) {
			case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
			case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
			case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
			case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
				texFlags |= TextureFlags::mipmap;
				break;
			default:;
			}
		} else {
			texFlags = TextureFlags::none;
		}

		auto texture = cachedTextures[id] = new Texture(Vec2u(gltfImage.width, gltfImage.height), 1, texFormat, texFlags);

		texture->update(gltfImage.image.data(), gltfImage.width * bytesPerTexel(texFormat));

		return texture;
	}

	Material* getMaterial(int id) {
		if (cachedMaterials[id]) return cachedMaterials[id];

		auto& gltfMat = gltfModel.materials[id];
		auto material = cachedMaterials[id] = new Material(&pbrMaterialDescriptor);

		material->setBlendMode(gltfMat.alphaMode == "BLEND" ? BlendMode::alpha : BlendMode::opaque);
		material->setCullMode(gltfMat.doubleSided ? CullMode::none : CullMode::front);

		auto& pbr = gltfMat.pbrMetallicRoughness;

		{
			auto factor = pbr.baseColorFactor.data();
			material->setVector4f("albedoColor4f",
								  Vec4f((float)factor[0], (float)factor[1], (float)factor[2], (float)factor[3]));
			//			CLOG << "alphaMode:" << gltfMat.alphaMode << "albedoColor:" << uniforms->albedoColor;
			auto& texInfo = pbr.baseColorTexture;
			if (texInfo.index >= 0) material->setTexture("albedoTexture", getTexture(texInfo.index));
		}
		{
			auto factor = gltfMat.emissiveFactor.data();
			material->setVector3f("emissiveColor3f", Vec3f((float)factor[0], (float)factor[1], (float)factor[2]));
			auto& texInfo = gltfMat.emissiveTexture;
			if (texInfo.index >= 0) material->setTexture("emissiveTexture", getTexture(texInfo.index));
		}
		{
			auto& texInfo = gltfMat.normalTexture;
			if (texInfo.index >= 0) {
				material->setTexture("normalTexture", getTexture(texInfo.index));
			}
		}
		{
			auto& texInfo = gltfMat.occlusionTexture;
			if (texInfo.index >= 0) material->setTexture("occlusionTexture", getTexture(texInfo.index));
		}
		{
			material->setFloat("metallicFactor1f", pbr.metallicFactor);
			material->setFloat("roughnessFactor1f", pbr.roughnessFactor);
			auto& texInfo = pbr.metallicRoughnessTexture;
			if (texInfo.index >= 0) {
				auto texture = getTexture(texInfo.index);
				material->setTexture("metallicTexture", texture);
				material->setTexture("roughnessTexture", texture);
			}
		}

		return material;
	}

	int componentsForType(int type) const {
		switch (type) {
		case TINYGLTF_TYPE_SCALAR:
			return 1;
		case TINYGLTF_TYPE_VEC2:
			return 2;
		case TINYGLTF_TYPE_VEC3:
			return 3;
		case TINYGLTF_TYPE_VEC4:
			return 4;
		default:
			SGD_ABORT();
		}
	}

	int bytesPerComponent(int componentType) const {
		switch (componentType) {
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			return 4;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			return 2;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			return 1;
		default:
			SGD_ABORT();
		}
	}

	uint8_t* bufferData(const tinygltf::Accessor& accessor) const {
		auto& bufferView = gltfModel.bufferViews[accessor.bufferView];
		auto& buffer = gltfModel.buffers[bufferView.buffer];
		return (uint8_t*)buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
	}

	int bufferDataPitch(const tinygltf::Accessor& accessor) const {
		auto& bufferView = gltfModel.bufferViews[accessor.bufferView];
		return accessor.ByteStride(bufferView);
	}

	void copyBufferData(const tinygltf::Accessor& accessor, void* dst, size_t dstPitch) const {
		auto srcp = bufferData(accessor);
		auto srcPitch = bufferDataPitch(accessor);
		int size = componentsForType(accessor.type) * bytesPerComponent(accessor.componentType);
		auto dstp = (uint8_t*)dst;

		for (int i = 0; i < accessor.count; ++i) {
			std::memcpy(dstp, srcp, size);
			dstp += dstPitch;
			srcp += srcPitch;
		}
	}

	void beginMesh() {
		this->vertices.clear();
		this->triangles.clear();
		this->hasNormals = false;
	}

	Mesh* endMesh() {

		Vector<Triangle> triangles;
		Vector<Surface> surfaces;

		auto flags = MeshFlags::none;

		for (auto it : this->triangles) {
			auto firstTri = (uint32_t)triangles.size();
			triangles.insert(triangles.end(), it.second.begin(), it.second.end());
			surfaces.push_back({getMaterial(it.first), firstTri, (uint32_t)it.second.size()});
			if (gltfModel.materials[it.first].normalTexture.index != -1) flags |= MeshFlags::tangentsEnabled;
		}

		auto mesh = new Mesh(vertices, triangles, surfaces, flags);

		if (bool(flags & MeshFlags::tangentsEnabled)) updateTangents(mesh);

		this->vertices.clear();
		this->triangles.clear();
		this->hasNormals = false;

		return mesh;
	}

	void updateMesh(const tinygltf::Primitive& gltfPrim) {
		if (gltfPrim.mode != TINYGLTF_MODE_TRIANGLES) return;
		if (gltfPrim.material == -1) return;

		// Add vertex attributes
		uint32_t firstVertex = vertices.size();
		uint32_t vertexCount = 0;
		for (auto& attrib : gltfPrim.attributes) {
			vertexCount = std::max(vertexCount, (uint32_t)gltfModel.accessors[attrib.second].count);
		}
		vertices.resize(firstVertex + vertexCount);
		auto vp = vertices.data() + firstVertex;

		for (auto& attrib : gltfPrim.attributes) {

			auto& accessor = gltfModel.accessors[attrib.second];
			int count = accessor.count;

			if (attrib.first == "POSITION") {
				SGD_ASSERT(accessor.type == TINYGLTF_TYPE_VEC3 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				copyBufferData(accessor, &vp->position, sizeof(Vertex));
				for (int i = 0; i < count; ++i) vp[i].position.z = -vp[i].position.z;
			} else if (attrib.first == "NORMAL") {
				SGD_ASSERT(accessor.type == TINYGLTF_TYPE_VEC3 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				copyBufferData(accessor, &vp->normal, sizeof(Vertex));
				for (int i = 0; i < count; ++i) {
					vp[i].normal.z = -vp[i].normal.z;
					vp[i].normal = normalize(vp[i].normal);
				}
				hasNormals = true;
			} else if (attrib.first == "TEXCOORD_0") {
				SGD_ASSERT(accessor.type == TINYGLTF_TYPE_VEC2 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				copyBufferData(accessor, &vp->texCoords, sizeof(Vertex));
			} else {
				// log() << String("### TODO: Unrecognized Gltf attribute \"") + attrib.first + "\"";
			}
		}

		// Add triangles
		const auto& accessor = gltfModel.accessors[gltfPrim.indices];

		auto& triangles = this->triangles[gltfPrim.material];
		uint32_t firstTri = triangles.size();
		uint32_t triCount = accessor.count / 3;
		SGD_ASSERT(triCount * 3 == accessor.count);

		triangles.resize(firstTri + triCount);
		auto tp = triangles.data() + firstTri;

		switch (accessor.componentType) {
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
			copyBufferData(accessor, tp, sizeof(uint32_t));
			break;
		}
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
			auto srcp = bufferData(accessor);
			auto srcPitch = bufferDataPitch(accessor);
			for (int i = 0; i < accessor.count; ++i) {
				((uint32_t*)tp)[i] = *((uint16_t*)srcp);
				srcp += srcPitch;
			}
			break;
		}
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
			auto srcp = bufferData(accessor);
			auto srcPitch = bufferDataPitch(accessor);
			for (int i = 0; i < accessor.count; ++i) {
				((uint32_t*)tp)[i] = *((uint8_t*)srcp);
				srcp += srcPitch;
			}
			break;
					}
		default:
			SGD_ABORT();
		}
		for (int i = 0; i < triCount; ++i) {
			std::swap(tp[i].indices[1], tp[i].indices[2]);
			tp[i].indices[0] += firstVertex;
			tp[i].indices[1] += firstVertex;
			tp[i].indices[2] += firstVertex;
		}
	}

	void updateMesh(const tinygltf::Mesh& gltfMesh) {
		for (const auto& gltfPrim : gltfMesh.primitives) {
			updateMesh(gltfPrim);
		}
	}

	void updateMesh(const tinygltf::Node& gltfNode, CAffineMat4f matrix) {

		auto worldMatrix = matrix * nodeMatrix(gltfNode);

		if (gltfNode.mesh >= 0) {
			auto firstVertex = vertices.size();
			updateMesh(gltfModel.meshes[gltfNode.mesh]);

			// transform added vertices...
			auto cof = cofactor(worldMatrix.r);
			for (auto vp = vertices.begin() + firstVertex; vp != vertices.end(); ++vp) {
				vp->position = worldMatrix * vp->position;
				vp->normal = normalize(cof * vp->normal);
			}
		}

		for (int i : gltfNode.children) {
			updateMesh(gltfModel.nodes[i], worldMatrix);
		}
	}

	Model* createModel(const tinygltf::Node& gltfNode, Model* parent) {

		Model* model = new Model();
		model->setParent(parent);
		model->setLocalMatrix(nodeMatrix(gltfNode));

		if (gltfNode.mesh >= 0) {
			beginMesh();
			updateMesh(gltfModel.meshes[gltfNode.mesh]);
			model->mesh = endMesh();
		}

		for (int i : gltfNode.children) {
			modelBones[i] = createModel(gltfModel.nodes[i], model);
		}

		return model;
	}

	Animation* createAnimation(const tinygltf::Animation& anim) const {

		log() << "### Create animation:" << anim.name << "channels:" << anim.channels.size();

		Map<int, AnimationSeqPtr> seqsMap;

		float duration = 0;

		for (auto& chan : anim.channels) {
			if (chan.target_path == "weights") continue; // TODO!

			int node = chan.target_node;
			auto it = seqsMap.find(node);
			if (it == seqsMap.end()) {
				it = seqsMap.insert(std::make_pair(node, new AnimationSeq(node))).first;
			}
			auto seq = it->second;

			auto& samp = anim.samplers[chan.sampler];

			// input == time
			auto& time_accessor = gltfModel.accessors[samp.input];
			SGD_ASSERT(time_accessor.type == TINYGLTF_TYPE_SCALAR &&
					   time_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			auto count = time_accessor.count;

			SGD_ASSERT(time_accessor.maxValues.size() == 1);
			duration = std::max(duration, (float)time_accessor.maxValues[0]);

			// output == pos / rot/ scale
			auto& value_accessor = gltfModel.accessors[samp.output];
			SGD_ASSERT(count == value_accessor.count);

			if (chan.target_path == "translation") {
				log() << "### translation keys:" << count;
				SGD_ASSERT(value_accessor.type == TINYGLTF_TYPE_VEC3 && //
						   value_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				seq->positionKeys.resize(count);
				copyBufferData(time_accessor, &seq->positionKeys[0].time, sizeof(seq->positionKeys[0]));
				copyBufferData(value_accessor, &seq->positionKeys[0].value, sizeof(seq->positionKeys[0]));
				for (auto& key : seq->positionKeys) key.value.z = -key.value.z;
			} else if (chan.target_path == "rotation") {
				log() << "### rotation keys:" << count;
				SGD_ASSERT(value_accessor.type == TINYGLTF_TYPE_VEC4 && //
						   value_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				seq->rotationKeys.resize(count);
				copyBufferData(time_accessor, &seq->rotationKeys[0].time, sizeof(seq->rotationKeys[0]));
				copyBufferData(value_accessor, &seq->rotationKeys[0].value, sizeof(seq->rotationKeys[0]));
				for (auto& key : seq->rotationKeys) key.value = Quatf(key.value.v, key.value.w);
			} else if (chan.target_path == "scale") {
				log() << "### scale keys:" << count;
				SGD_ASSERT(value_accessor.type == TINYGLTF_TYPE_VEC3 && //
						   value_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				seq->scaleKeys.resize(count);
				copyBufferData(time_accessor, &seq->scaleKeys[0].time, sizeof(seq->scaleKeys[0]));
				copyBufferData(value_accessor, &seq->scaleKeys[0].value, sizeof(seq->scaleKeys[0]));
			} else {
				SGD_ABORT();
			}
		}
		Vector<CAnimationSeqPtr> seqs;
		for (auto it : seqsMap) seqs.push_back(it.second);
		return new Animation(anim.name, seqs, duration);
	}

	// ***** Public API *****

	Mesh* createMesh() {
		beginMesh();
		for (auto i : defaultScene->nodes) {
			updateMesh(gltfModel.nodes[i], {});
		}
		return endMesh();
	}

	Model* createModel() {

		Vector<AnimationPtr> animations;
		for (auto& anim : gltfModel.animations) {
			animations.push_back(createAnimation(anim));
		}

		for (int i : defaultScene->nodes) {
			modelBones[i] = createModel(gltfModel.nodes[i], nullptr);
		}

		auto root = new Model(modelBones, animations);

		for (int i : defaultScene->nodes) {
			modelBones[i]->setParent(root);
		}
		return root;
	}
};

} // namespace

Expected<Mesh*, FileioEx> loadGLTFMesh(CPath path) {

	GLTFLoader loader{};
	auto res = loader.load(path);
	if (!res) return res.error();

	return loader.createMesh();
}

Expected<Model*, FileioEx> loadGLTFModel(CPath path) {

	GLTFLoader loader{};
	auto res = loader.load(path);
	if (!res) return res.error();

	return loader.createModel();
}

} // namespace sgd
