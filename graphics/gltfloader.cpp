#include "gltfloader.h"

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

void transformMesh(Vector<Vertex>::iterator begin, Vector<Vertex>::iterator end, CAffineMat4f matrix) {
	auto cofactor = sgd::cofactor(matrix.r);

	for (auto it = begin; it != end; ++it) {
		it->position = matrix * it->position;
		it->normal = normalize(cofactor * it->normal);
	}
}

struct GLTFLoader {

	tinygltf::Model gltfModel;
	Vector<Vertex> loadedVertices;
	Vector<Vector<Triangle>> loadedTriangles; // keyed by material index

	void loadPrimitive(const tinygltf::Primitive& gltfPrim) {
		if (gltfPrim.material < 0) return;

		auto materialId = gltfPrim.material;

		// Vertices
		size_t v0 = loadedVertices.size();
		size_t vcount = 0;
		for (auto& attrib : gltfPrim.attributes) {
			vcount = std::max(vcount, gltfModel.accessors[attrib.second].count);
		}
		loadedVertices.resize(v0 + vcount);

		for (auto& attrib : gltfPrim.attributes) {
			auto& accessor = gltfModel.accessors[attrib.second];

			size_t count = accessor.count;
			auto vp = loadedVertices.data() + v0;

			const auto& bufferView = gltfModel.bufferViews[accessor.bufferView];
			const auto& buffer = gltfModel.buffers[bufferView.buffer];
			uint8_t* src = (uint8_t*)buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

			int stride = accessor.ByteStride(bufferView);
			int size = (accessor.type != TINYGLTF_TYPE_SCALAR) ? accessor.type : 1;

			// CLOG << "Loading attrib" << attrib.first;

			if (attrib.first == "POSITION") {
				SGD_ASSERT(size == 3 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				for (size_t i = 0; i < count; ++i) {
					vp->position = *(Vec3f*)src;
					vp->position.z = -vp->position.z;
					src += stride;
					++vp;
				}
			} else if (attrib.first == "NORMAL") {
				SGD_ASSERT(size == 3 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				for (size_t i = 0; i < count; ++i) {
					vp->normal = *(Vec3f*)src;
					vp->normal.z = -vp->normal.z;
					vp->normal = normalize(vp->normal);
					src += stride;
					++vp;
				}
			} else if (attrib.first == "TEXCOORD_0") {
				SGD_ASSERT(size == 2 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
				for (size_t i = 0; i < count; ++i) {
					vp->texCoords = *(Vec2f*)src;
					src += stride;
					++vp;
				}
			} else {
				// log() << String("### TODO: Unrecognized Gltf attribute \"") + attrib.first + "\"";
			}
		}

		// Triangles
		const auto& accessor = gltfModel.accessors[gltfPrim.indices];

		const auto& bufferView = gltfModel.bufferViews[accessor.bufferView];
		const auto& buffer = gltfModel.buffers[bufferView.buffer];
		uint8_t* data = (uint8_t*)buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
		auto stride = accessor.ByteStride(bufferView);

		auto& triangles = loadedTriangles[materialId];
		size_t firstTri = triangles.size();
		size_t triCount = accessor.count / 3;
		SGD_ASSERT(triCount * 3 == accessor.count);
		triangles.resize(firstTri + triCount);
		auto tri = triangles.data() + firstTri;

		switch (accessor.componentType) {
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
			SGD_ASSERT(stride == 4);
			auto src = (uint32_t*)data;
			for (size_t i = 0; i < triCount; ++i) {
				tri->indices[0] = src[0] + v0;
				tri->indices[1] = src[2] + v0;
				tri->indices[2] = src[1] + v0;
				src += 3;
				++tri;
			}
			break;
		}
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
			SGD_ASSERT(stride == 2);
			auto src = (uint16_t*)data;
			for (size_t i = 0; i < triCount; ++i) {
				tri->indices[0] = src[0] + v0;
				tri->indices[1] = src[2] + v0;
				tri->indices[2] = src[1] + v0;
				src += 3;
				++tri;
			}
			break;
		}
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
			SGD_ASSERT(stride == 1);
			auto src = (uint8_t*)data;
			for (size_t i = 0; i < triCount; ++i) {
				tri->indices[0] = src[0] + v0;
				tri->indices[1] = src[2] + v0;
				tri->indices[2] = src[1] + v0;
				src += 3;
				++tri;
			}
			break;
		}
		default:
			SGD_ABORT();
		}
	}

	void loadMesh(const tinygltf::Mesh& gltfMesh) {

		//		CLOG << "Loading mesh" << gltfMesh.name;

		for (const auto& gltfPrim : gltfMesh.primitives) {
			if (gltfPrim.mode != TINYGLTF_MODE_TRIANGLES) continue;

			loadPrimitive(gltfPrim);
		}
	}

	void loadNode(const tinygltf::Node& gltfNode, CAffineMat4f matrix) { // NOLINT (recursive)

		//		CLOG << "Loading node" << gltfNode.name;

		auto worldMatrix = matrix * nodeMatrix(gltfNode);

		if (gltfNode.mesh >= 0) {
			SGD_ASSERT(gltfNode.mesh < gltfModel.meshes.size());

			auto v0 = (ptrdiff_t)loadedVertices.size();
			loadMesh(gltfModel.meshes[gltfNode.mesh]);
			transformMesh(loadedVertices.begin() + v0, loadedVertices.end(), worldMatrix);
		}

		for (int i : gltfNode.children) {
			SGD_ASSERT(i < gltfModel.nodes.size());

			loadNode(gltfModel.nodes[i], worldMatrix);
		}
	}

	void loadScene(const tinygltf::Scene& gltfScene) {

		//		CLOG << "Loading scene" << gltfScene.name;

		for (int i : gltfScene.nodes) {
			SGD_ASSERT(i < gltfModel.nodes.size());

			loadNode(gltfModel.nodes[i], {});
		}
	}

	// Can be called on any thread!
	bool loadData(CData data, CString baseDir) {
		std::string err;
		std::string warn;
		tinygltf::TinyGLTF gltfLoader;

		bool res;
		auto magic = "glTF";
		if (data.size() >= 4 && !std::memcmp(data.data(), magic, 4)) {
			res = gltfLoader.LoadBinaryFromMemory(&gltfModel, &err, &warn, data.data(), data.size());
		} else {
			res = gltfLoader.LoadASCIIFromString(&gltfModel, &err, &warn, (char*)data.data(), data.size(), baseDir);
		}
		if (!err.empty()) log() << "!!! Tiny gltf error:" << err;
		if (!warn.empty()) log() << "!!! Tiny gltf warning: " << warn;
		if (!res) {
			SGD_PANIC("Failed to load GLTF data");
			return false;
		}

		loadedTriangles.resize(gltfModel.materials.size());

		SGD_ASSERT(gltfModel.defaultScene >= 0);
		loadScene(gltfModel.scenes[gltfModel.defaultScene]);
		return true;
	}

	// ***** Must be called on main thread! *****
	//
	Vector<bool> loadedImages;
	Vector<TexturePtr> loadedTextures;

	Texture* loadTexture(int index) {
		SGD_ASSERT(index >= 0 && index < gltfModel.textures.size());

		if (loadedTextures[index]) return loadedTextures[index];

		auto& gltfTex = gltfModel.textures[index];

		SGD_ASSERT(gltfTex.source >= 0 && gltfTex.source < gltfModel.images.size());
		auto& gltfImage = gltfModel.images[gltfTex.source];
		if (gltfImage.bits != 8) SGD_PANIC("gltfImage.bits: " + std::to_string(gltfImage.bits));
		if (gltfImage.component != 4) SGD_PANIC("gltfImage.component: " + std::to_string(gltfImage.component));
		auto texFormat = TextureFormat::srgba8;

		if (!loadedImages[gltfTex.source]) {
			premultiplyAlpha(gltfImage.image.data(), TextureFormat::srgba8, Vec2u(gltfImage.width, gltfImage.height),
							 gltfImage.width * bytesPerTexel(texFormat));
			loadedImages[gltfTex.source] = true;
		}

		TextureFlags texFlags{};
		if (gltfTex.sampler >= 0) {
			SGD_ASSERT(gltfTex.sampler < gltfModel.samplers.size());
			auto& gltfSampler = gltfModel.samplers[gltfTex.sampler];
			if (gltfSampler.wrapS == TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE) texFlags |= TextureFlags::clampU;
			if (gltfSampler.wrapT == TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE) texFlags |= TextureFlags::clampV;
			if (gltfSampler.magFilter == TINYGLTF_TEXTURE_FILTER_LINEAR) texFlags |= TextureFlags::magFilter;
			switch (gltfSampler.minFilter) {
			case TINYGLTF_TEXTURE_FILTER_NEAREST:
			case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
				break;
			case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
				texFlags |= TextureFlags::minFilter;
				break;
			case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
				texFlags |= TextureFlags::mipFilter;
				break;
			default:
				texFlags |= TextureFlags::minFilter;
				texFlags |= TextureFlags::mipFilter;
				break;
			}
		} else {
			texFlags = TextureFlags::none;
		}

		auto texture = new Texture(Vec2u(gltfImage.width, gltfImage.height), 1, texFormat, texFlags);

		texture->update(gltfImage.image.data(), gltfImage.width * bytesPerTexel(texFormat));

		loadedTextures[index] = texture;

		return texture;
	}

	Material* loadMaterial(const tinygltf::Material& gltfMat) {

		// CLOG << "Loading material:" << gltfMat.name << "alphaMode:" << gltfMat.alphaMode
		//	 << "doubleSided:" << gltfMat.doubleSided;

		auto material = new Material(&pbrMaterialDescriptor);
		material->setBlendMode(gltfMat.alphaMode == "BLEND" ? BlendMode::alpha : BlendMode::opaque);
		material->setCullMode(gltfMat.doubleSided ? CullMode::none : CullMode::back);

		auto& pbr = gltfMat.pbrMetallicRoughness;

		{
			auto factor = pbr.baseColorFactor.data();
			material->setVector4f("albedoColor4f",
								  Vec4f((float)factor[0], (float)factor[1], (float)factor[2], (float)factor[3]));
			//			CLOG << "alphaMode:" << gltfMat.alphaMode << "albedoColor:" << uniforms->albedoColor;
			auto& texInfo = pbr.baseColorTexture;
			if (texInfo.index >= 0) material->setTexture("albedoTexture", loadTexture(texInfo.index));
		}
		{
			auto factor = gltfMat.emissiveFactor.data();
			material->setVector3f("emissiveColor3f", Vec3f((float)factor[0], (float)factor[1], (float)factor[2]));
			auto& texInfo = gltfMat.emissiveTexture;
			if (texInfo.index >= 0) material->setTexture("emissiveTexture", loadTexture(texInfo.index));
		}
		{
			auto& texInfo = gltfMat.normalTexture;
			if (texInfo.index >= 0) material->setTexture("normalTexture", loadTexture(texInfo.index));
		}
		{
			auto& texInfo = gltfMat.occlusionTexture;
			if (texInfo.index >= 0) material->setTexture("occlusionTexture", loadTexture(texInfo.index));
		}
		{
			material->setFloat("metallicFactor1f", pbr.metallicFactor);
			material->setFloat("roughnessFactor1f", pbr.roughnessFactor);
			auto& texInfo = pbr.metallicRoughnessTexture;
			if (texInfo.index >= 0) {
				auto texture = loadTexture(texInfo.index);
				material->setTexture("metallicTexture", texture);
				material->setTexture("roughnessTexture", texture);
			}
		}

		return material;
	}

	Mesh* createMesh() {

		loadedImages.resize(gltfModel.images.size());
		loadedTextures.resize(gltfModel.textures.size());

		Vector<Triangle> triangles;
		Vector<Surface> surfaces;

		for (int i = 0; i < loadedTriangles.size(); ++i) {
			auto& tris = loadedTriangles[i];
			if (tris.empty()) continue;
			auto material = loadMaterial(gltfModel.materials[i]);
			triangles.insert(triangles.end(), tris.begin(), tris.end());
			surfaces.push_back({material, (uint32_t)tris.size()});
		}

		auto mesh = new Mesh(loadedVertices.data(), loadedVertices.size(), //
							 triangles.data(), triangles.size(), //
							 MeshFlags::tangentsEnabled);

		for(auto& surf : surfaces) mesh->addSurface(surf);

		return mesh;
	}
};

} // namespace

Expected<Mesh*, FileioEx> loadGLTFMesh(CPath path) {
	GLTFLoader loader{};
	auto baseDir = path.resolve().parent_path().u8string();
	log() << "### baseDir:" << baseDir;
	loader.loadData(loadData(path).result(), baseDir);
	return loader.createMesh();
}

} // namespace sgd
