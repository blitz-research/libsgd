#include "gltfloader.h"

#include <graphics/exports.h>

#define TINYGLTF_IMPLEMENTATION 1
#define TINYGLTF_NO_IMAGE_WRITE 1
#include <tiny_gltf.h>

namespace sgd {

namespace {

SGD_BOOL_CONFIG_VAR(g_loggingEnabled, "gltf.loggingEnabled", false);

} // namespace

AffineMat4f GLTFLoader::nodeMatrix(const tinygltf::Node& gltfNode) {

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

int GLTFLoader::componentsForType(int type) {
	static const Map<int, int> map{{TINYGLTF_TYPE_SCALAR, 1}, {TINYGLTF_TYPE_VEC2, 2}, {TINYGLTF_TYPE_VEC3, 3}, //
								   {TINYGLTF_TYPE_VEC4, 4},	  {TINYGLTF_TYPE_MAT2, 4}, {TINYGLTF_TYPE_MAT3, 9}, //
								   {TINYGLTF_TYPE_MAT4, 16}};
	auto it = map.find(type);
	SGD_ASSERT(it != map.end());
	return it->second;
}

int GLTFLoader::bytesPerComponent(int componentType) {
	static const Map<int, int> map{{TINYGLTF_COMPONENT_TYPE_FLOAT, 4},
								   {TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, 4},
								   {TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, 2},
								   {TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, 1}};
	auto it = map.find(componentType);
	SGD_ASSERT(it != map.end());
	return it->second;
}

int GLTFLoader::byteSize(const tinygltf::Accessor& accessor) {
	return componentsForType(accessor.type) * bytesPerComponent(accessor.componentType);
}

Expected<bool, FileioEx> GLTFLoader::open(CPath path) {

	auto rdata = loadData(path);
	if (!rdata) return rdata.error();

	auto data = rdata.result();

	std::string err;
	std::string warn;
	tinygltf::TinyGLTF gltfLoader;

	bool res;
	auto magic = "glTF";
	auto baseDir = path.parentPath().str();

	// Load!
	if (data.size() >= 4 && !std::memcmp(data.data(), magic, 4)) {
		res = gltfLoader.LoadBinaryFromMemory(&gltfModel, &err, &warn, data.data(), data.size(), baseDir);
	} else {
		res = gltfLoader.LoadASCIIFromString(&gltfModel, &err, &warn, (char*)data.data(), data.size(), baseDir);
	}
	// Error?
	if (!res || !err.empty()) {
		if (err.empty()) err = "Unknown error";
		SGD_LOG << "Tiny gltf error:" << err;
		return SGD_FILEIOEX("Tiny gltf error: " + err);
	}
	// Warning?
	if (!warn.empty()) {
		SGD_LOG << "Tiny gltf warning:" << warn;
	}

	cachedImages.resize(gltfModel.images.size());
	cachedTextures.resize(gltfModel.textures.size());
	cachedMaterials.resize(gltfModel.materials.size());

	meshVertices.reserve(512 * 1024); // Too much?

	return true;
}

Texture* GLTFLoader::loadTexture(int id, TextureFormat format, bool pmAlpha) {
	if (cachedTextures[id]) return cachedTextures[id];

	auto& gltfTex = gltfModel.textures[id];
	auto& gltfImage = gltfModel.images[gltfTex.source];

	// Shouldn't just PANIC!
	if (gltfImage.bits != 8) SGD_ERROR("TODO: Unsupported gltfImage.bits: " + toString(gltfImage.bits));
	if (gltfImage.component != 4) SGD_ERROR("TODO: Unsupported gltfImage.component: " + toString(gltfImage.component));

	auto flags = TextureFlags::none;
	if (gltfTex.sampler >= 0) {
		SGD_ASSERT(gltfTex.sampler < gltfModel.samplers.size());
		auto& gltfSampler = gltfModel.samplers[gltfTex.sampler];
		if (gltfSampler.wrapS == TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE) flags |= TextureFlags::clampU;
		if (gltfSampler.wrapT == TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE) flags |= TextureFlags::clampV;

		switch (gltfSampler.magFilter) {
		case TINYGLTF_TEXTURE_FILTER_LINEAR:
			flags |= TextureFlags::filter;
			break;
		default: {
		}
		}
		switch (gltfSampler.minFilter) {
		case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
		case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
		case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
		case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
			flags |= TextureFlags::mipmap;
			break;
		default: {
		}
		}
	} else {
		flags = TextureFlags::none;
	}

	auto data = new TextureData(Vec2u(gltfImage.width, gltfImage.height), format);
	std::memcpy(data->data(), gltfImage.image.data(), data->pitch() * data->size().y);
	if (pmAlpha) premultiplyAlpha(data);

	if (g_loggingEnabled)
		SGD_LOG << "  Creating texture:" << gltfTex.name << "size:" << data->size() << "format:" << (int)data->format()
				<< "flags:" << (int)flags;

	return cachedTextures[id] = new Texture(data->size(), 1, data->format(), flags, data);
}

Material* GLTFLoader::loadMaterial(int id) {
	if (cachedMaterials[id]) return cachedMaterials[id];

	auto& gltfMat = gltfModel.materials[id];
	auto material = cachedMaterials[id] = new Material(&pbrMaterialDescriptor);

	static const Map<String, BlendMode> blendModes{{"OPAQUE", BlendMode::opaque},  //
												   {"MASK", BlendMode::alphaMask}, //
												   {"BLEND", BlendMode::alphaBlend}};

	auto it = blendModes.find(gltfMat.alphaMode);
	material->blendMode = it != blendModes.end() ? it->second : BlendMode::opaque;
	material->depthFunc = DepthFunc::lessEqual;
	material->cullMode = gltfMat.doubleSided ? CullMode::none : CullMode::back;

	if (g_loggingEnabled) {
		SGD_LOG << "Creating material:" << gltfMat.name <<	   //
			"blendMode:" << toString(material->blendMode()) << //
			"depthFunc:" << toString(material->depthFunc()) << //
			"cullMode:" << toString(material->cullMode());
		// SGD_LOG << "  Material extensions:";
		// for (auto& [key, value] : gltfMat.extensions) {
		//	SGD_LOG << "  extension name:" << key;
		// }
	}

	auto& pbr = gltfMat.pbrMetallicRoughness;

	auto blendMode = material->blendMode();

	// BaseColor color / texture
	{
		auto factor = pbr.baseColorFactor.data();
		auto color = Vec4f((float)factor[0], (float)factor[1], (float)factor[2], (float)factor[3]);
		if (g_loggingEnabled) SGD_LOG << "  baseColorfactor:" << color;

		material->setColor("albedo", toSRGBAColor(color));

		auto& texInfo = pbr.baseColorTexture;
		if (texInfo.index >= 0) {
			if (texInfo.texCoord == 0) {
				if (g_loggingEnabled) SGD_LOG << "  baseColorTexture:" << gltfModel.textures[texInfo.index].name;
				auto texture = loadTexture(texInfo.index, TextureFormat::srgba8, material->blendMode() != BlendMode::alphaMask);
				material->setTexture("albedo", texture);
			} else {
				SGD_LOG << "TODO: Skipping baseColor texture with texInfo.texCoord != 0" << texInfo.texCoord;
			}
		}
	}

	// Emissive color / texture
	{
		auto factor = gltfMat.emissiveFactor.data();
		auto color = Vec4f((float)factor[0], (float)factor[1], (float)factor[2], 1);
		if (g_loggingEnabled) SGD_LOG << "  emissiveFactor:" << color;

		// Look for emissiveStrength extension supported by at least blender + gltf2 exporter.
		auto ext = gltfMat.extensions.find("KHR_materials_emissive_strength");
		if (ext != gltfMat.extensions.end()) {
			auto& val = ext->second;
			if (val.Has("emissiveStrength")) {
				if (val.Get("emissiveStrength").IsNumber()) {
					auto strength = (float)val.Get("emissiveStrength").GetNumberAsDouble();
					if (g_loggingEnabled) SGD_LOG << "  emissiveStrength:" << strength;
					color *= strength;
				}
			}
		}

		material->setColor("emissive", toSRGBAColor(color));

		auto& texInfo = gltfMat.emissiveTexture;
		if (texInfo.index >= 0) {
			if (texInfo.texCoord == 0) {
				if (g_loggingEnabled) SGD_LOG << "  emissiveTexture:" << gltfModel.textures[texInfo.index].name;
				auto texture = loadTexture(texInfo.index, TextureFormat::srgba8, false);
				material->setTexture("emissive", texture);
			} else {
				SGD_LOG << "Todo: Skipping emissive texture with texInfo.texCoord != 0";
			}
		}
	}

	// MetallicRoughness textures / factors.
	{
		if (g_loggingEnabled) SGD_LOG << "  metallicFactor:" << pbr.metallicFactor;
		material->setFloat("metallic", (float)pbr.metallicFactor);

		if (g_loggingEnabled) SGD_LOG << "  roughnessFactor:" << pbr.roughnessFactor;
		material->setFloat("roughness", (float)pbr.roughnessFactor);

		auto& texInfo = pbr.metallicRoughnessTexture;
		if (texInfo.index >= 0) {
			if (texInfo.texCoord == 0) {
				if (g_loggingEnabled) SGD_LOG << "  metallicRoughness texture:" << gltfModel.textures[texInfo.index].name;
				auto texture = loadTexture(texInfo.index, TextureFormat::rgba8, false);
				material->setTexture("metallic", texture);
				material->setTexture("roughness", texture);
			} else {
				SGD_LOG << "TODO: Skipping metallicRoughness texture with texInfo.texCoord != 0" << texInfo.texCoord;
			}
		}
	}

	// Normal texture
	{
		auto& texInfo = gltfMat.normalTexture;
		if (texInfo.index >= 0) {
			if (texInfo.texCoord == 0) {
				// SGD_ASSERT(texInfo.scale == 1.0f);
				if (g_loggingEnabled) SGD_LOG << "  normalTexture:" << gltfModel.textures[texInfo.index].name;
				material->setTexture("normal", loadTexture(texInfo.index, TextureFormat::rgba8, false));
			} else {
				SGD_LOG << "TODO: Skipping normal texture with texInfo.texCoord != 0" << texInfo.texCoord;
			}
		}
	}

	// Occlusion texture
	{
		auto& texInfo = gltfMat.occlusionTexture;
		if (texInfo.index >= 0) {
			if (texInfo.texCoord == 0) {
				SGD_ASSERT(texInfo.texCoord == 0);
				if (g_loggingEnabled) SGD_LOG << "  occlusionTexture:" << gltfModel.textures[texInfo.index].name;
				material->setTexture("occlusion", loadTexture(texInfo.index, TextureFormat::rgba8, false));
			} else {
				SGD_LOG << "TODO: Skipping occlusion texture with texInfo.texCoord != 0" << texInfo.texCoord;
			}
		}
	}

	return material;
}

uint8_t* GLTFLoader::bufferData(const tinygltf::Accessor& accessor) const {
	auto& bufferView = gltfModel.bufferViews[accessor.bufferView];
	auto& buffer = gltfModel.buffers[bufferView.buffer];
	return (uint8_t*)buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
}

int GLTFLoader::bufferDataPitch(const tinygltf::Accessor& accessor) const {
	auto& bufferView = gltfModel.bufferViews[accessor.bufferView];
	return accessor.ByteStride(bufferView);
}

template <class T, class D>
void GLTFLoader::copyBufferData(const tinygltf::Accessor& accessor, void* dst, size_t dstPitch) const {
	int size = byteSize(accessor);
	SGD_ASSERT(size == sizeof(T));
	auto dstp = (uint8_t*)dst;
	auto srcp = bufferData(accessor);
	auto srcPitch = bufferDataPitch(accessor);
	for (int i = 0; i < accessor.count; ++i) {
		*(D*)dstp = (D)(*(T*)srcp);
		dstp += dstPitch;
		srcp += srcPitch;
	}
}

void GLTFLoader::copyBufferData(const tinygltf::Accessor& accessor, void* dst, size_t dstPitch) const {
	auto size = byteSize(accessor);
	auto srcp = bufferData(accessor);
	auto srcPitch = bufferDataPitch(accessor);
	auto dstp = (uint8_t*)dst;
	for (int i = 0; i < accessor.count; ++i) {
		std::memcpy(dstp, srcp, size);
		dstp += dstPitch;
		srcp += srcPitch;
	}
}

void GLTFLoader::beginMesh() {
	meshVertices.clear();
	meshSurfaces.clear();
	opaqueSurfaces.clear();
	meshHasTangents = false;
	meshFlags = MeshFlags::none;
}

Mesh* GLTFLoader::endMesh() {

	if (g_loggingEnabled) {
		SGD_LOG << "Creating mesh vertices:" << meshVertices.size() << "surfaces:" << meshSurfaces.size()
				<< "flags:" << (int)meshFlags;
	}

	auto mesh = new Mesh(meshVertices.size(), meshFlags);
	sgd::copy(mesh->lockVertices(), meshVertices.data(), meshVertices.size());
	mesh->unlockVertices();

	for (auto& surf : meshSurfaces) {
		auto& triangles = surf.triangles;
		auto surface = new Surface(mesh, loadMaterial(surf.materialId), triangles.size());
		sgd::copy(surface->lockTriangles(), triangles.data(), triangles.size());
		surface->unlockTriangles();
		mesh->addSurface(surface);
	}

	if (bool(meshFlags & MeshFlags::tangentsEnabled) && !meshHasTangents) {
		SGD_LOG << "### UpdateTangents";
		updateTangents(mesh);
	}

	mesh->updateBounds();

	beginMesh();

	return mesh;
}

void GLTFLoader::updateMesh(const tinygltf::Primitive& gltfPrim) {
	if (gltfPrim.mode != TINYGLTF_MODE_TRIANGLES) {
		SGD_LOG << "TODO: Skipping unsupported gltf primitive mode:" << gltfPrim.mode;
		return;
	}
	if (gltfPrim.material == -1) {
		return;
	}

	// Add vertex attributes
	uint32_t firstVertex = meshVertices.size();
	uint32_t vertexCount = 0;
	for (auto& attrib : gltfPrim.attributes) {
		vertexCount = std::max(vertexCount, (uint32_t)gltfModel.accessors[attrib.second].count);
	}
	if (firstVertex + vertexCount > meshVertices.capacity()) {
		meshVertices.reserve(std::max((uint32_t)meshVertices.capacity() * 3 / 2, vertexCount));
	}
	meshVertices.resize(firstVertex + vertexCount);
	auto vp = meshVertices.data() + firstVertex;

	for (auto& attrib : gltfPrim.attributes) {

		auto& accessor = gltfModel.accessors[attrib.second];
		auto count = accessor.count;

		if (attrib.first == "POSITION") {
			//
			if (accessor.type == TINYGLTF_TYPE_VEC3) {
				if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
					copyBufferData(accessor, &vp->position, sizeof(Vertex));
				} else {
					SGD_LOG << "TODO: Unsupported POSITION component type:" << accessor.componentType;
					continue;
				}
			} else {
				SGD_LOG << "TODO: Unsupported POSITION type:" << accessor.type;
				continue;
			}
			for (int i = 0; i < count; ++i) vp[i].position.z = -vp[i].position.z;
			//
		} else if (attrib.first == "NORMAL") {
			//
			if (accessor.type == TINYGLTF_TYPE_VEC3) {
				if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
					copyBufferData(accessor, &vp->normal, sizeof(Vertex));
				} else {
					SGD_LOG << "TODO: Unsupported NORMAL component type:" << accessor.componentType;
					continue;
				}
			} else {
				SGD_LOG << "TODO: Unsupported NORMAL type:" << accessor.type;
				continue;
			}
			for (int i = 0; i < count; ++i) vp[i].normal.z = -vp[i].normal.z;
			//
		} else if (attrib.first == "TANGENT") {
			//
			if (accessor.type == TINYGLTF_TYPE_VEC4) {
				if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
					copyBufferData(accessor, &vp->tangent, sizeof(Vertex));
				} else {
					SGD_LOG << "TODO: Unsupported TANGENT component type:" << accessor.componentType;
					continue;
				}
			} else {
				SGD_LOG << "TODO: Unsupported TANGENT type:" << accessor.type;
				continue;
			}
			for (int i = 0; i < count; ++i) vp[i].tangent.z = -vp[i].tangent.z;
			meshHasTangents = true;
			//
		} else if (attrib.first == "TEXCOORD_0") {
			//
			if (accessor.type == TINYGLTF_TYPE_VEC2) {
				if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
					copyBufferData(accessor, &vp->texCoords, sizeof(Vertex));
				} else {
					SGD_LOG << "TODO: Unsupported TEXCOORD_0 component type:" << accessor.componentType;
					continue;
				}
			} else {
				SGD_LOG << "TODO: Unsupported TEXCOORD_0 type:" << accessor.type;
				continue;
			}
			//
		} else if (attrib.first == "COLOR_0") {
			//
			if (accessor.type == TINYGLTF_TYPE_VEC4) {
				if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
					copyBufferData(accessor, &vp->color, sizeof(Vertex));
				} else {
					SGD_LOG << "TODO: Unsupported COLOR_0 component type:" << accessor.componentType;
					continue;
				}
			} else {
				SGD_LOG << "TODO: Unsupported COLOR_0 type:" << accessor.type;
				continue;
			}
			for (int i = 0; i < count; ++i) vp[i].color.xyz() *= vp[i].color.w;
			//
		} else if (attrib.first == "JOINTS_0") {
			//
			if (accessor.type == TINYGLTF_TYPE_VEC4) {
				if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
					copyBufferData<Vec4<uint8_t>, Vec4<uint8_t>>(accessor, &vp->joints, sizeof(Vertex));
				} else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
					copyBufferData<Vec4<int16_t>, Vec4<uint8_t>>(accessor, &vp->joints, sizeof(Vertex));
				} else {
					SGD_LOG << "TODO: Unsupported JOINTS_0 component type:" << accessor.componentType;
					continue;
				}
			} else {
				SGD_LOG << "TODO: Unsupported JOINTS_0 type:" << accessor.type;
				continue;
			}
#if SGD_CONFIG_DEBUG
			// Validate joint indices
			for (int i = 0; i < accessor.count; ++i) {
				SGD_ASSERT(vp[i].joints[0] < bones.size() && vp[i].joints[1] < bones.size() && vp[i].joints[2] < bones.size() &&
						   vp[i].joints[3] < bones.size());
			}
#endif //
		} else if (attrib.first == "WEIGHTS_0") {
			//
			if (accessor.type == TINYGLTF_TYPE_VEC4) {
				if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
					copyBufferData(accessor, &vp->weights, sizeof(Vertex));
				} else {
					SGD_LOG << "TODO: Unsupported WEIGHTS_0 component type:" << accessor.componentType;
					continue;
				}
			} else {
				SGD_LOG << "TODO: Unsupported WEIGHTS_0 type:" << accessor.type;
				continue;
			}
#if SGD_CONFIG_DEBUG
			for (int i = 0; i < accessor.count; ++i) {
				float w = vp[i].weights[0] + vp[i].weights[1] + vp[i].weights[2] + vp[i].weights[3];
				SGD_ASSERT(std::abs(w - 1) < .00001f);
			}
#endif //
		} else {
			SGD_LOG << "TODO: Skipping unrecognized gltf attribute:" << attrib.first;
		}
	}

	auto& gltfMat = gltfModel.materials[gltfPrim.material];
	if (gltfMat.normalTexture.index != -1 && gltfMat.normalTexture.texCoord == 0) meshFlags |= MeshFlags::tangentsEnabled;

	MeshSurface* surfp = nullptr;
	if (gltfMat.alphaMode != "BLEND") {
		// shared surface
		auto it = opaqueSurfaces.find(gltfPrim.material);
		if (it != opaqueSurfaces.end()) {
			surfp = &meshSurfaces[it->second];
		} else {
			meshSurfaces.push_back({gltfPrim.material});
			surfp = &meshSurfaces.back();
			opaqueSurfaces.insert(std::make_pair(gltfPrim.material, (uint32_t)meshSurfaces.size() - 1));
		}
	} else {
		meshSurfaces.push_back({gltfPrim.material});
		surfp = &meshSurfaces.back();
		meshFlags |= MeshFlags::blendedSurfaces;
	}
	auto& triangles = surfp->triangles;

	// Add triangles
	const auto& accessor = gltfModel.accessors[gltfPrim.indices];

	uint32_t firstTri = triangles.size();
	uint32_t triCount = accessor.count / 3;
	SGD_ASSERT(triCount * 3 == accessor.count);

	triangles.resize(firstTri + triCount);
	auto tp = (uint32_t*)(triangles.data() + firstTri);

	switch (accessor.componentType) {
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
		copyBufferData(accessor, tp, sizeof(uint32_t));
		break;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		copyBufferData<uint16_t, uint32_t>(accessor, tp, sizeof(uint32_t));
		break;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
		copyBufferData<uint8_t, uint32_t>(accessor, tp, sizeof(uint32_t));
		break;
	default:
		SGD_ERROR("TODO: Unsupported gltf component type for triangle indices");
	}
	for (int i = 0; i < accessor.count; ++i) {
		SGD_ASSERT(tp[i] < vertexCount);
		tp[i] += firstVertex;
	}
}

void GLTFLoader::updateMesh(const tinygltf::Mesh& gltfMesh) {
	for (const auto& gltfPrim : gltfMesh.primitives) {
		updateMesh(gltfPrim);
	}
}

void GLTFLoader::loadAnimations() {
	SGD_ASSERT(animations.empty());

	for (auto& anim : gltfModel.animations) {
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
				SGD_ASSERT(value_accessor.type == TINYGLTF_TYPE_VEC3 && //
						   value_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				seq->positionKeys.resize(count);
				copyBufferData(time_accessor, &seq->positionKeys[0].time, sizeof(seq->positionKeys[0]));
				copyBufferData(value_accessor, &seq->positionKeys[0].value, sizeof(seq->positionKeys[0]));
				for (auto& key : seq->positionKeys) key.value.z = -key.value.z;
			} else if (chan.target_path == "rotation") {
				SGD_ASSERT(value_accessor.type == TINYGLTF_TYPE_VEC4 && //
						   value_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				seq->rotationKeys.resize(count);
				copyBufferData(time_accessor, &seq->rotationKeys[0].time, sizeof(seq->rotationKeys[0]));
				copyBufferData(value_accessor, &seq->rotationKeys[0].value, sizeof(seq->rotationKeys[0]));
				for (auto& key : seq->rotationKeys) key.value = {{-key.value.v.x, -key.value.v.y, key.value.v.z}, -key.value.w};
			} else if (chan.target_path == "scale") {
				SGD_ASSERT(value_accessor.type == TINYGLTF_TYPE_VEC3 && //
						   value_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				seq->scaleKeys.resize(count);
				copyBufferData(time_accessor, &seq->scaleKeys[0].time, sizeof(seq->scaleKeys[0]));
				copyBufferData(value_accessor, &seq->scaleKeys[0].value, sizeof(seq->scaleKeys[0]));
			} else {
				SGD_ERROR("TODO: Unsupported gltf animation channel target_path");
			}
		}
		Vector<CAnimationSeqPtr> seqs;
		for (auto it : seqsMap) seqs.push_back(it.second);
		animations.push_back(new Animation(anim.name, seqs, duration));
	}
}

Model* GLTFLoader::loadBones(const tinygltf::Node& gltfNode, Model* parent) {

	if (g_loggingEnabled) SGD_LOG << "Creating Model for node:" << gltfNode.name;

	auto model = new Model();
	model->setName(gltfNode.name);
	model->setParent(parent);
	model->setLocalMatrix(nodeMatrix(gltfNode));

	for (int i : gltfNode.children) {
		bones[i] = loadBones(gltfModel.nodes[i], model);
	}

	return model;
}

void GLTFLoader::loadBones() {
	SGD_ASSERT(bones.empty());

	bones.resize(gltfModel.nodes.size());

	for (int i = 0; i < bones.size(); ++i) {
		rootBones.insert(i);
	}

	Function<void(int)> visit;

	visit = [&](int i) {
		auto& node = gltfModel.nodes[i];
		for (int c : node.children) {
			if (!rootBones.count(c)) continue;
			rootBones.erase(c);
			visit(c);
		}
	};

	for (int i = 0; i < bones.size(); ++i) {
		visit(i);
	}

	for (int i : rootBones) {
		bones[i] = loadBones(gltfModel.nodes[i], nullptr);
	}
}

void GLTFLoader::loadJoints() {
	SGD_ASSERT(!gltfModel.skins.empty());
	SGD_ASSERT(joints.empty());

	auto& skin = gltfModel.skins[0];

	joints.resize(skin.joints.size());

	if (skin.inverseBindMatrices != -1) {
		auto& accessor = gltfModel.accessors[skin.inverseBindMatrices];
		SGD_ASSERT(accessor.count == joints.size() && accessor.type == TINYGLTF_TYPE_MAT4 &&
				   accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
		Vector<Mat4f> matrices(joints.size());
		copyBufferData(accessor, &matrices[0], sizeof(Mat4f));
		for (int i = 0; i < joints.size(); ++i) {
			auto r = &matrices[i].i.x;
			auto& m = joints[i].inverseBindMatrix;

			m.r.i = {r[0], r[1], -r[2]};
			m.r.j = {r[4], r[5], -r[6]};
			m.r.k = {-r[8], -r[9], r[10]};
			m.t = {r[12], r[13], -r[14]};
		}
	}
	for (int i = 0; i < joints.size(); ++i) joints[i].bone = skin.joints[i];
}

Expected<Mesh*, FileioEx> GLTFLoader::loadStaticMesh() {

	loadBones();

	beginMesh();

	for (int i = 0; i < bones.size(); ++i) {
		auto mesh = gltfModel.nodes[i].mesh;
		if (mesh == -1) continue;

		auto firstVertex = (uint32_t)meshVertices.size();

		updateMesh(gltfModel.meshes[mesh]);

		const auto& worldMatrix = bones[i] ? bones[i]->worldMatrix() : AffineMat4r{};
		auto cof = cofactor(worldMatrix.r);

		for (auto vp = meshVertices.begin() + firstVertex; vp != meshVertices.end(); ++vp) {
			vp->position = worldMatrix * Vec3r(vp->position);
			vp->normal = normalize(cof * Vec3r(vp->normal));
			vp->tangent.xyz() = cof * Vec3r(vp->tangent.xyz());
		}
	}

	return endMesh();
}

Expected<Model*, FileioEx> GLTFLoader::loadBonedModel() {

	loadBones();

	loadAnimations();

	for (int i = 0; i < bones.size(); ++i) {
		auto mesh = gltfModel.nodes[i].mesh;
		if (mesh == -1 || !bones[i]) continue;

		if (g_loggingEnabled) SGD_LOG << "Loading mesh for bone" << gltfModel.nodes[i].name;

		beginMesh();
		updateMesh(gltfModel.meshes[mesh]);
		bones[i]->as<Model>()->mesh = endMesh();
	}

	auto model = new Model(bones, animations, {});

	for (int i : rootBones) {
		bones[i]->setParent(model);
	}

	return model;
}

Expected<Model*, FileioEx> GLTFLoader::loadSkinnedModel() {
	if (gltfModel.skins.empty()) return SGD_FILEIOEX("Model contains no skins");

	loadBones();

	loadAnimations();

	loadJoints();

	beginMesh();
	for (int i = 0; i < bones.size(); ++i) {
		if (gltfModel.nodes[i].skin != 0) continue;

		auto mesh = gltfModel.nodes[i].mesh;
		if (mesh == -1) continue;

		updateMesh(gltfModel.meshes[mesh]);
	}
	auto skin = endMesh();

	auto model = new Model(bones, animations, joints);
	model->mesh = skin;

	for (int i : rootBones) {
		bones[i]->setParent(model);
	}

	return model;
}

Expected<Mesh*, FileioEx> loadStaticMesh(CPath path) {
	GLTFLoader loader;

	auto r = loader.open(path);
	if (!r) return r.error();

	if (g_loggingEnabled) SGD_LOG << "Loading static mesh" << path.str();

	return loader.loadStaticMesh();
}

Expected<Model*, FileioEx> loadBonedModel(CPath path) {
	GLTFLoader loader;

	auto r = loader.open(path);
	if (!r) return r.error();

	if (g_loggingEnabled) SGD_LOG << "Loading boned model" << path.str();

	return loader.loadBonedModel();
}

Expected<Model*, FileioEx> loadSkinnedModel(CPath path) {
	GLTFLoader loader;

	auto r = loader.open(path);
	if (!r) return r.error();

	if (g_loggingEnabled) SGD_LOG << "Loading skinned model" << path.str();

	return loader.loadSkinnedModel();
}

} // namespace sgd
