#include "surface.h"
#include "mesh.h"

namespace sgd {

Surface::Surface(CMesh* mesh, Material* material, uint32_t triangleCount)
	: m_mesh(mesh),			//
	  m_material(material), //
	  m_triangleCount(triangleCount),
	  m_triangleBuffer(new Buffer(BufferType::index, nullptr, triangleCount * sizeof(Triangle))) {
}

void Surface::resizeTriangles(uint32_t count) {
	m_triangleBuffer->resize(count * sizeof(Triangle));
	m_triangleCount = count;
}

void Surface::updateBounds() {
	auto vp = m_mesh->vertices();
	m_bounds = {};
	auto tp = triangles();
	for (int i = 0; i < m_triangleCount; ++i) {
		m_bounds |= vp[tp[i].indices[0]].position;
		m_bounds |= vp[tp[i].indices[1]].position;
		m_bounds |= vp[tp[i].indices[2]].position;
	}
	m_origin = center(m_bounds);
}

} // namespace sgd
