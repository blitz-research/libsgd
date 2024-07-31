#pragma once

#include <graphics/exports.h>

#include "model.h"

#include <tiny_gltf.h>

namespace sgd {

// TODO: Move this to .cpp
struct GLTFLoader {

private:

	struct MeshSurface {
		int materialId;
		Vector<Triangle> triangles;
	};

	tinygltf::Model gltfModel;

	Vector<bool> cachedImages;
	Vector<TexturePtr> cachedTextures;
	Vector<MaterialPtr> cachedMaterials;

	Vector<Vertex> meshVertices;
	Vector<MeshSurface> meshSurfaces;
	Map<int, uint32_t> opaqueSurfaces;		// keyed by material index, index into meshSurfaces
	bool meshHasTangents{};
	MeshFlags meshFlags;

	// loadBones
	Vector<EntityPtr> bones;
	Set<int> rootBones;
	Vector<AnimationPtr> animations;
	Vector<Joint> joints;

	static AffineMat4f nodeMatrix(const tinygltf::Node& gltfNode);

	static int componentsForType(int type);
	static int bytesPerComponent(int componentType);
	static int byteSize(const tinygltf::Accessor& accessor);

	Texture* loadTexture(int id, bool srgb, bool pmAlpha);
	Material* loadMaterial(int id);

	uint8_t* bufferData(const tinygltf::Accessor& accessor) const;
	int bufferDataPitch(const tinygltf::Accessor& accessor) const;
	void copyBufferData(const tinygltf::Accessor& accessor, void* dst, size_t dstPitch) const;
	template <class T, class D> void copyBufferData(const tinygltf::Accessor& accessor, void* dst, size_t dstPitch) const;

	void beginMesh();
	Mesh* endMesh();

	void updateMesh(const tinygltf::Primitive& gltfPrim);
	void updateMesh(const tinygltf::Mesh& gltfMesh);

	Model* loadBones(const tinygltf::Node& gltfNode, Model* parent);

	void loadBones();
	void loadAnimations();
	void loadJoints();

public:

	Expected<bool, FileioEx> open(CPath path);

	Expected<Mesh*, FileioEx> loadStaticMesh();
	Expected<Model*, FileioEx> loadBonedModel();
	Expected<Model*, FileioEx> loadSkinnedModel();
};

Expected<Mesh*, FileioEx> loadStaticMesh(CPath path);

Expected<Model*, FileioEx> loadBonedModel(CPath path);

Expected<Model*, FileioEx> loadSkinnedModel(CPath path);

} // namespace sgd
