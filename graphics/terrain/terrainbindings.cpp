#include "terrainbindings.h"

#include "../core/textureutil.h"
#include "../material/prelitmaterial.h"
#include "../render/renderqueue.h"

namespace sgd {

namespace {

uint32_t rndColor(uint32_t lod) {
	auto r = (uint32_t)rnd(128)+128;
	uint32_t color;
	switch (lod) {
	case 0:
		color = r;
		break;
	case 1:
		color = r << 8;
		break;
	case 2:
		color = r << 16;
		break;
	case 3:
		color = r << 8 | r;
		break;
	case 4:
		color = r << 16 | r;
		break;
	case 5:
		color = r << 16 | r << 8;
		break;
	default:
		color = r << 16 | r << 8 | r;
		break;
	}
	return color | 0xff000000u;
}

} // namespace

TerrainBindings::TerrainBindings()
	: m_bindGroup(new BindGroup(&terrainBindGroupDescriptor)), //
	  m_uniformBuffer(new Buffer(BufferType::uniform, nullptr, sizeof(TerrainUniforms))) {

	m_bindGroup->setBuffer(0, m_uniformBuffer);

	(void)worldMatrix.changed.connect(nullptr, [=](CAffineMat4f m){
		lockUniforms().worldMatrix = m;
		unlockUniforms();
	});

	(void)lodLevels.changed.connect(nullptr, [=](uint32_t n) {
		lockUniforms().lodLevels = n;
		unlockUniforms();
		invalidate();
	});
	lodLevels = 4;

	(void)quadsPerTile.changed.connect(nullptr, [=](uint32_t n) {
		lockUniforms().quadsPerTile = n;
		unlockUniforms();
		invalidate();
	});
	quadsPerTile = 16;

	(void)heightTexture.changed.connect(nullptr, [=](CTexture* ttexture) { m_bindGroup->setTexture(1, ttexture); });
	heightTexture = blackTexture(TextureFlags::none);

	(void)normalTexture.changed.connect(nullptr, [=](CTexture* ttexture) { m_bindGroup->setTexture(3, ttexture); });
	normalTexture = flatTexture(TextureFlags::none);

	material = new Material(&prelitMaterialDescriptor);
	material()->cullMode=CullMode::none;
}

void TerrainBindings::onValidate() const {

	using Vertex = TerrainVertex;

	Vector<Vertex> vertices;
	Vector<uint32_t> indices;

	vertices.reserve(1024 * 1024 * 16);
	indices.reserve(1024 * 1024 * 16);

	auto quadsPerTile = this->quadsPerTile();

	for (uint32_t lod = 0; lod < lodLevels(); ++lod) {

		auto scale = float(1 << lod);
		auto tileSize = float(quadsPerTile * 4) * scale;

		for (uint32_t v = 0; v < 4; ++v) {

			for (uint32_t h = 0; h < 4; ++h) {

				// Leave 'hole' in center of 4X4 tiles.
				if (lod && h == 1 && (v == 1 || v == 2)) h = 3;

				for (uint32_t j = 0; j < quadsPerTile; j += 2) {

					for (uint32_t i = 0; i < quadsPerTile; i += 2) {

						float x = float(h * quadsPerTile + i) * scale - tileSize / 2;
						float y = float(v * quadsPerTile + j) * scale - tileSize / 2;

						static constexpr uint32_t tris[]{
							// clang-format off
							0,9,1,  1,9,4,  4,9,3,  3,9,0,
							1,10,2, 2,10,5, 5,10,4, 4,10,1,
							3,11,4, 4,11,7, 7,11,6, 6,11,3,
							4,12,5, 5,12,8, 8,12,7, 7,12,4
							//0,1,9,1,4,9,4,3,9,3,0,9,
							//1,2,10,2,5,10,5,4,10,4,1,10,
							//3,4,11,4,7,11,7,6,11,6,3,11,
							//4,5,12,5,8,12,8,7,12,7,4,12
							// clang-format on
						};
						uint32_t v0 = vertices.size();
						indices.reserve(indices.size() + std::size(tris));
						for (uint32_t k : tris) {
							indices.push_back(k + v0);
						}

						static constexpr Vec2f verts[]{
							// clang-format off
							Vec2f(0, 0), Vec2f(1, 0), Vec2f(2, 0),
							Vec2f(0, 1), Vec2f(1, 1), Vec2f(2, 1),
							Vec2f(0, 2), Vec2f(1, 2), Vec2f(2, 2),
							Vec2f(.5f, .5f), Vec2f(1.5f, .5f),
							Vec2f(.5f, 1.5f), Vec2f(1.5f, 1.5f)
							// clang-format on
						};
						auto color = rndColor(lod);
						for (CVec2f tv : verts) {
							vertices.emplace_back(Vec3f(x + tv.x * scale, (float)lod, -(y + tv.y * scale)), color);
						}
					}
				}
			}
		}
	}

	log() << "### Terrain mesh vertices:" << vertices.size() << "triangles:" << indices.size() / 3;

	m_vertexBuffer = new Buffer(BufferType::vertex, vertices.data(), sizeof(Vertex) * vertices.size());

	m_indexBuffer = new Buffer(BufferType::index, indices.data(), sizeof(uint32_t) * indices.size());

	m_indexCount = indices.size();
}

void TerrainBindings::render(RenderQueue* rq) const {
	rq->addRenderOp(m_vertexBuffer, m_indexBuffer, material(), m_bindGroup, emptyBindGroup(BindGroupType::renderer), //
					m_indexCount, 1, 0, 0, shadowsEnabled());
}

} // namespace sgd
