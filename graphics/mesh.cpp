#include "mesh.h"

namespace sgd {

Mesh::Mesh(const Vertex* vertices, uint32_t vertexCount, const Triangle* triangles, uint32_t triangleCount,
		   CVector<Surface> surfaces, MeshFlags flags)
	: m_vertexBuffer(new Buffer(BufferType::vertex, vertices, vertexCount * sizeof(Vertex))),		//
	  m_vertexCount(vertexCount),																	//
	  m_triangleBuffer(new Buffer(BufferType::index, triangles, triangleCount * sizeof(Triangle))), //
	  m_triangleCount(triangleCount),																//
	  m_surfaces(surfaces),																			//
	  m_flags(flags) {

	addDependency(m_vertexBuffer);
	addDependency(m_triangleBuffer);
}

Mesh::Mesh(CVector<Vertex> vertices, CVector<Triangle> triangles, CVector<Surface> surfaces, MeshFlags flags)
	: //
	  Mesh(vertices.data(), vertices.size(), triangles.data(), triangles.size(), surfaces, flags) {
}

void Mesh::resizeVertices(uint32_t size) {
	if (size == m_vertexCount) return;

	m_vertexCount = size;
	m_vertexBuffer->resize(m_vertexCount * sizeof(Vertex));
}

void Mesh::resizeTriangles(uint32_t size) {
	if (size == m_triangleCount) return;

	m_triangleCount = size;
	m_triangleBuffer->resize(m_triangleCount * sizeof(Triangle));
}

void Mesh::addVertices(const Vertex* first, uint32_t count) {
	uint32_t size = (m_vertexCount + count) * sizeof(Vertex);
	if (size > m_vertexBuffer->size()) m_vertexBuffer->resize(size);
	if (first) m_vertexBuffer->update(first, m_vertexCount * sizeof(Vertex), count * sizeof(Vertex));
	m_vertexCount += count;
}

void Mesh::addTriangles(const Triangle* first, uint32_t count) {
	uint32_t size = (m_triangleCount + count) * sizeof(Triangle);
	if (size > m_triangleBuffer->size()) m_triangleBuffer->resize(size);
	if (first) m_triangleBuffer->update(first, m_triangleCount * sizeof(Triangle), count * sizeof(Triangle));
	m_triangleCount += count;
}

Vertex* Mesh::lockVertices(uint32_t first, uint32_t count) {
	return (Vertex*)m_vertexBuffer->lock(first * sizeof(Vertex), count * sizeof(Vertex));
}

void Mesh::unlockVertices() {
	m_vertexBuffer->unlock();
}

Triangle* Mesh::lockTriangles(uint32_t first, uint32_t count) {
	return (Triangle*)m_triangleBuffer->lock(first * sizeof(Triangle), count * sizeof(Triangle));
}

void Mesh::unlockTriangles() {
	m_triangleBuffer->unlock();
}

void Mesh::clearSurfaces() {
	if (m_surfaces.empty()) return;

	for (auto& surf : m_surfaces) removeDependency(surf.material);

	m_surfaces.clear();

	invalidate(true);
}

void Mesh::addSurface(CSurface surface) {

	m_surfaces.push_back(surface);

	addDependency(surface.material);

	invalidate(true);
}

} // namespace sgd
