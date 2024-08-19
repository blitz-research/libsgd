#include "drawlist.h"

#include "../material/prelitmaterial.h"
#include "../render/renderqueue.h"

namespace sgd {

namespace {

struct DrawListUniforms {
	Mat4f matrix;
};

auto shaderSource{
#include "drawlistrenderer.wgsl"
};

wgpu::VertexAttribute vertexBufferAttribs[]{
	{wgpu::VertexFormat::Float32x3, 0, 0},	// Vec3f position
	{wgpu::VertexFormat::Float32x3, 12, 1}, // Vec3f texCoords0
	{wgpu::VertexFormat::Float32x4, 24, 2}, // Vec4f color
};

static_assert(sizeof(DrawList::Vertex) == 40);

wgpu::VertexBufferLayout const vertexBufferLayout{sizeof(DrawList::Vertex), wgpu::VertexStepMode::Vertex,
												  std::size(vertexBufferAttribs), vertexBufferAttribs};

BindGroupDescriptor bindGroupDescriptor( //
	"drawListGeomtry",					 //
	BindGroupType::geometry,			 //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
								   wgpu::BufferBindingType::Uniform),
	},
	shaderSource,		  //
	{vertexBufferLayout}, //
	wgpu::PrimitiveTopology::TriangleList);

} // namespace

DrawList::DrawList()
	: m_bindGroup(new BindGroup(&bindGroupDescriptor)), //
	  m_uniformBuffer(new Buffer(BufferType::uniform, nullptr, sizeof(DrawListUniforms))),
	  m_vertexBuffer(new Buffer(BufferType::vertex, nullptr, m_vertexCapacity * sizeof(Vertex))), font(defaultFont()) {

	DrawListUniforms uniforms;
	m_uniformBuffer->update(&uniforms, 0, sizeof(uniforms));

	m_bindGroup->setBuffer(0, m_uniformBuffer);

	auto cmaterial = new Material(&prelitMaterialDescriptor);
	cmaterial->blendMode = BlendMode::alphaBlend;
	//	cmaterial->depthFunc = DepthFunc::always;
	cmaterial->cullMode = CullMode::none;
	m_whiteMaterial = cmaterial;

	fillMaterial = m_whiteMaterial;

	projectionMatrix.changed.connect(this, [=](CMat4f matrix) { //
		m_uniformBuffer->update(&matrix, offsetof(DrawListUniforms, matrix), sizeof(matrix));
	});
	projectionMatrix = Mat4f::ortho(0, 1920, 1080, 0, 0, 1);

	fillColor.changed.connect(this, [=](CVec4f color) { //
		m_pmFillColor = Vec4f(Vec3f(color) * color.w, color.w);
	});
	m_pmFillColor = fillColor();

	outlineColor.changed.connect(this, [=](CVec4f color) { //
		m_pmOutlineColor = Vec4f(Vec3f(color) * color.w, color.w);
	});
	m_pmOutlineColor = outlineColor();

	textColor.changed.connect(this, [=](CVec4f color) { //
		m_pmTextColor = Vec4f(Vec3f(color) * color.w, color.w);
	});
	m_pmTextColor = textColor();

	m_drawOpCounts.push_back(0);
	m_vertexCounts.push_back(0);

	clear();
}

DrawList::Vertex* DrawList::allocTriangles(uint32_t count, CMaterial* material) {
	SGD_ASSERT(material);
	if (material != m_drawOp.material) {
		if (m_drawOp.triangleCount) m_drawOps.push_back(m_drawOp);
		m_drawOp.material = material;
		m_drawOp.triangleCount = count;
	} else {
		m_drawOp.triangleCount += count;
	}
	auto first = m_vertexCount;
	auto vcount = count * 3;
	m_vertexCount += vcount;
	if (m_vertexCount > m_vertexCapacity) {
		m_vertexCapacity = m_vertexCount;
		m_vertexBuffer->resize(m_vertexCapacity * sizeof(Vertex));
	}
	return (Vertex*)m_vertexBuffer->lock(first * sizeof(Vertex), vcount * sizeof(Vertex));
}

void DrawList::clear() {
	m_drawOps.resize(m_drawOpCounts.back());
	m_vertexCount = m_vertexCounts.back();
	m_drawOp.material = {};
	m_drawOp.triangleCount = 0;
}

void DrawList::flush() {
	if (m_drawOp.triangleCount) {
		m_drawOps.push_back(m_drawOp);
	}
	m_drawOp.material = {};
	m_drawOp.triangleCount = 0;
}

void DrawList::pushLayer() {
	flush();
	m_drawOpCounts.push_back(m_drawOps.size());
	m_vertexCounts.push_back(m_vertexCount);
}

void DrawList::popLayer() {
	if (m_drawOpCounts.size() == 1) SGD_ERROR("DrawList layer stack underflow");
	clear();
	m_drawOpCounts.pop_back();
	m_vertexCounts.pop_back();
}

void DrawList::addPoint(CVec2f v) {
	float hsz = pointSize() * .5f;
	addOval({v - hsz, v + hsz});
}

void DrawList::fillQuad(CVec2f v0, CVec2f v1, CVec2f v2, CVec2f v3, CMaterial* material, CVec4f color, float frame) {
	Vec3f tl{v0, depth()};
	Vec3f tr{v1, depth()};
	Vec3f br{v2, depth()};
	Vec3f bl{v3, depth()};
	auto vp = allocTriangles(2, material);
	vp[0] = {tl, {0, 0, frame}, color};
	vp[1] = {br, {1, 1, frame}, color};
	vp[2] = {tr, {1, 0, frame}, color};
	vp[3] = vp[0];
	vp[4] = {bl, {0, 1, frame}, color};
	vp[5] = vp[1];
	m_vertexBuffer->unlock();
}

void DrawList::fillLine(CVec2f v0, CVec2f v1, CMaterial* material, CVec4f color, float width, float frame) {
	auto dv = normalize(v1 - v0) * width * .5f;
	auto tv = Vec2f(-dv.y, dv.x);
	return fillQuad(v0 - tv, v0 + tv, v1 + tv, v1 - tv, material, color, frame);
}

void DrawList::fillRect(CRectf rect, CMaterial* material, CVec4f color, float frame) {
	return fillQuad(topLeft(rect), topRight(rect), bottomRight(rect), bottomLeft(rect), material, color, frame);
}

void DrawList::addOutline(CVec2f v0, CVec2f v1) {
	fillLine(v0, v1, m_whiteMaterial, m_pmOutlineColor, outlineWidth(), 0);
}

void DrawList::addOutline(CRectf rect) {
	auto tl = topLeft(rect);
	auto tr = topRight(rect);
	auto br = bottomRight(rect);
	auto bl = bottomLeft(rect);
	addOutline(tl, tr);
	addOutline(tr, br);
	addOutline(br, bl);
	addOutline(bl, tl);
}

void DrawList::addImage(CImage* image, CVec2f v, float frame) {
	auto size = Vec2f(image->texture()->size());
	auto rect = image->rect();
	rect.min *= size;
	rect.max *= size;
	rect += v;

	if (fillEnabled()) fillRect(rect, image->material(), m_pmFillColor, frame);
	//if (outlineEnabled()) addOutline(rect);
}

void DrawList::addRect(CRectf rect) {
	if (fillEnabled()) fillRect(rect, fillMaterial(), m_pmFillColor, 0);
	if (outlineEnabled()) addOutline(rect);
}

void DrawList::addLine(CVec2f v0, CVec2f v1) {
	auto dv = normalize(v1 - v0) * lineWidth() * .5f;
	auto tv = Vec2f(-dv.y, dv.x);
	auto tl = v0 - tv;
	auto tr = v0 + tv;
	auto br = v1 + tv;
	auto bl = v1 - tv;
	if (fillEnabled()) fillQuad(v0 - tv, v0 + tv, v1 + tv, v1 - tv, fillMaterial(), m_pmFillColor, 0);
	if (!outlineEnabled()) return;
	addOutline(tl, tr);
	addOutline(tr, br);
	addOutline(br, bl);
	addOutline(bl, tl);
}

void DrawList::addOval(CRectf rect) {

	auto v = center(rect);
	auto r = rect.size() * .5f;

	float d = length(r);
	int n = std::max(int(d * 2), 12) & ~3;

	constexpr float off = 0; //.5f;

	if (fillEnabled()) {

		auto vp = allocTriangles(n, fillMaterial());

		float th0 = (off)*twoPi / (float)n;
		Vec2f v0{v.x + std::cos(th0) * r.x, v.y + std::sin(th0) * r.y};

		for (int i = 0; i < n; ++i) {
			float th1 = (i + 1 + off) * twoPi / (float)n;
			Vec2f v1{v.x + std::cos(th1) * r.x, v.y + std::sin(th1) * r.y};

			vp[0] = {{v0, depth()}, {0, 0, 0}, m_pmFillColor};
			vp[2] = {{v1, depth()}, {0, 0, 0}, m_pmFillColor};
			vp[1] = {{v, depth()}, {0, 0, 0}, m_pmFillColor};

			v0 = v1;
			vp += 3;
		}
		m_vertexBuffer->unlock();
	}
	if (outlineEnabled()) {

		float th0 = (off)*twoPi / (float)n;
		Vec2f v0{v.x + std::cos(th0) * r.x, v.y + std::sin(th0) * r.y};

		for (int i = 0; i < n; ++i) {
			float th1 = (i + 1 + off) * twoPi / (float)n;
			Vec2f v1{v.x + std::cos(th1) * r.x, v.y + std::sin(th1) * r.y};

			addOutline(v0, v1);

			v0 = v1;
		}
	}
}

void DrawList::addText(CString text, CVec2f v) {

	Vertex* vp = allocTriangles(text.size() * 2, font()->atlas);

	Vec2f pos(v.x, v.y + font()->ascent);

	for (uint8_t ch : text) {

		CGlyph glyph = font()->glyph(ch);

		CRectf src = glyph.srcRect;
		Rectf dst = glyph.dstRect + pos;

		vp[0] = {{dst.min.x, dst.min.y, depth()}, {src.min.x, src.min.y, 0}, m_pmTextColor};
		vp[1] = {{dst.max.x, dst.max.y, depth()}, {src.max.x, src.max.y, 0}, m_pmTextColor};
		vp[2] = {{dst.max.x, dst.min.y, depth()}, {src.max.x, src.min.y, 0}, m_pmTextColor};
		vp[3] = vp[0];
		vp[4] = {{dst.min.x, dst.max.y, depth()}, {src.min.x, src.max.y, 0}, m_pmTextColor};
		vp[5] = vp[1];

		vp += 6;

		pos.x += glyph.advance;
	}
	m_vertexBuffer->unlock();
}

void DrawList::render(RenderQueue* rq) const {

	uint32_t first = 0;

	auto renderer = emptyBindGroup(BindGroupType::renderer);

	for (auto& drawOp : m_drawOps) {

		auto count = drawOp.triangleCount * 3;
		auto material = drawOp.material ? drawOp.material : m_whiteMaterial;

		rq->addRenderOp(m_vertexBuffer, nullptr,		 //
						material, m_bindGroup, renderer, //
						count, 1, first, 0, false);		 //

		first += count;
	}
}

} // namespace sgd
