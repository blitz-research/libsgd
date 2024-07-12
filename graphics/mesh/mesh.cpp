#include "mesh.h"

namespace sgd {

namespace {

auto shaderSource =
#include "meshrenderer.wgsl"
	;

wgpu::VertexAttribute vertexBufferAttribs[]{
	{wgpu::VertexFormat::Float32x3, 0, 0},	// Vec3f position
	{wgpu::VertexFormat::Float32x3, 12, 1}, // Vec3f normal
	{wgpu::VertexFormat::Float32x4, 24, 2}, // Vec4f tangent
	{wgpu::VertexFormat::Float32x3, 40, 3}, // Vec3f texCoords
	{wgpu::VertexFormat::Float32x4, 52, 4}, // Vec4f color
											// uchar8_t joints[4];
											// float weights[4];
};
static_assert(sizeof(Vertex) == 88);

wgpu::VertexBufferLayout const vertexBufferLayout{sizeof(Vertex), wgpu::VertexStepMode::Vertex, std::size(vertexBufferAttribs),
												  vertexBufferAttribs};

BindGroupDescriptor meshGeometryDesc( //
	"meshGeometry",					  //
	BindGroupType::geometry,		  //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
								   wgpu::BufferBindingType::Uniform),
	},
	shaderSource,							//
	{vertexBufferLayout},					//
	wgpu::PrimitiveTopology::TriangleList); //

} // namespace

// ***** Surface

Surface::Surface(uint32_t triangleCount, Material* material)
	: m_triangleBuffer(new Buffer(BufferType::index, nullptr, triangleCount * sizeof(Triangle))),
	  m_triangleCount(triangleCount), m_material(material) {
}

void Surface::resizeTriangles(uint32_t count) {
	m_triangleBuffer->resize(count * sizeof(Triangle));
	m_triangleCount = count;
}

// ***** Mesh *****

Mesh::Mesh(uint32_t vertexCount, MeshFlags flags)
	: m_vertexBuffer(new Buffer(BufferType::vertex, nullptr, vertexCount * sizeof(Vertex))), m_vertexCount(vertexCount),
	  m_flags(flags), m_bindGroup(new BindGroup(&meshGeometryDesc)) {

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
