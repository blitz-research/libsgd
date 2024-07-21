#include "mesh.h"

namespace sgd {

namespace {

BindGroupDescriptor bindGroupDescriptor( //
	"meshGeometry",						//
	BindGroupType::geometry,			//
	{
		// geometry_uniforms: MeshUniforms
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
								   wgpu::BufferBindingType::Uniform),
	});

} // namespace

// ***** Surface *****

Surface::Surface(uint32_t triangleCount, Material* material)
	: m_triangleBuffer(new Buffer(BufferType::index, nullptr, triangleCount * sizeof(Triangle))),
	  m_triangleCount(triangleCount), m_material(material) {
}

void Surface::resizeTriangles(uint32_t count) {
	m_triangleBuffer->resize(count * sizeof(Triangle));
	m_triangleCount = count;
}

// ***** Mesh *****

Mesh::Mesh(uint32_t vertexCount, MeshFlags flags)											 //
	: m_bindGroup(new BindGroup(&bindGroupDescriptor)),										 //
	  m_vertexBuffer(new Buffer(BufferType::vertex, nullptr, vertexCount * sizeof(Vertex))), //
	  m_vertexCount(vertexCount),															 //
	  m_flags(flags) {

	MeshUniforms uniforms;
	uniforms.tangentsEnabled = bool(m_flags & MeshFlags::tangentsEnabled);
	m_bindGroup->setBuffer(0, new Buffer(BufferType::uniform, &uniforms, sizeof(uniforms)));
}

void Mesh::resizeVertices(uint32_t count) {
	m_vertexBuffer->resize(count * sizeof(Vertex));
	m_vertexCount = count;
}

void Mesh::clearSurfaces() {
	for (Surface* surf : m_surfaces) {
		removeDependency(surf->triangleBuffer());
		removeDependency(surf->material());
	}
	m_surfaces.clear();
	invalidate(true);
}

void Mesh::addSurface(Surface* surf) {
	m_surfaces.emplace_back(surf);
	addDependency(surf->triangleBuffer());
	addDependency(surf->material());
}

} // namespace sgd
