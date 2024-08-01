#include "mesh.h"

namespace sgd {

namespace {

BindGroupDescriptor bindGroupDescriptor( //
	"meshGeometry",						 //
	BindGroupType::geometry,			 //
	{
		// geometry_uniforms: MeshUniforms
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
								   wgpu::BufferBindingType::Uniform),
	});

} // namespace

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

void Mesh::updateBounds() {
	m_bounds = {};
	for (Surface* surf : m_surfaces) {
		surf->updateBounds();
		m_bounds |= surf->bounds();
	}
	m_origin = center(m_bounds);
}

} // namespace sgd
