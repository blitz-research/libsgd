#include "drawlist.h"

#include "prelitmaterial.h"

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/drawlistrenderer.wgsl"
};

wgpu::VertexAttribute vertexBufferAttribs[]{
	{wgpu::VertexFormat::Float32x3, 0, 0},	// Vec3f position
	{wgpu::VertexFormat::Float32x2, 12, 1}, // Vec3f texCoords0
	{wgpu::VertexFormat::Float32x4, 20, 2}, // Vec4f color
};
static_assert(sizeof(DrawList::Vertex) == 36);

wgpu::VertexBufferLayout const vertexBufferLayout{sizeof(DrawList::Vertex), wgpu::VertexStepMode::Vertex,
												  std::size(vertexBufferAttribs), vertexBufferAttribs};

BindGroupDescriptor bindGroupDescriptor //
	(2,
	 {
		 bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
									wgpu::BufferBindingType::Uniform),
	 },
	 {vertexBufferLayout}, //
	 shaderSource);
} // namespace

DrawList::DrawList()
	: m_bindGroup(new BindGroup(&bindGroupDescriptor)), //
	  m_uniformBuffer(new Buffer(BufferType::uniform, nullptr, sizeof(DrawListUniforms))),
	  m_vertexBuffer(new Buffer(BufferType::vertex, nullptr, m_vertexCapacity * sizeof(Vertex))) {

	addDependency(m_bindGroup);
	addDependency(m_vertexBuffer);

	m_bindGroup->setBuffer(0, m_uniformBuffer);

	DrawListUniforms uniforms;
	//	uniforms.matrix = Mat4f{};//Mat4f::ortho(0, 1920, 1080, 0, 0, 1);
	uniforms.matrix = Mat4f::ortho(0, 1920, 1080, 0, 0, 1);
	m_uniformBuffer->update(&uniforms, 0, sizeof(uniforms));

	auto cmaterial = new Material(&prelitMaterialDescriptor);
	cmaterial->blendMode = BlendMode::alpha;
	cmaterial->depthFunc = DepthFunc::always;
	cmaterial->cullMode = CullMode::none;
	m_defaultMaterial = cmaterial;

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

	clear();
} // ,

DrawList::Vertex* DrawList::allocTriangles(uint32_t count) {
	if (fillMaterial() != m_drawOp.material) {
		if (m_drawOp.triangleCount) m_drawOps.push_back(m_drawOp);
		m_drawOp.material = fillMaterial();
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
	m_drawOps.clear();
	m_drawOp.triangleCount = 0;
	m_vertexCount = 0;
	invalidate();
}

void DrawList::flush() {
	if (m_drawOp.triangleCount) {
		m_drawOps.push_back(m_drawOp);
		invalidate();
	}
	m_drawOp.triangleCount = 0;
}

void DrawList::addRect(float x0, float y0, float x1, float y1) {
	if (m_pmFillColor.w > 0.0f) {
		auto v = allocTriangles(2);
		v[0] = {{x0, y0, depth()}, {0, 0}, m_pmFillColor};
		v[1] = {{x1, y0, depth()}, {1, 0}, m_pmFillColor};
		v[2] = {{x1, y1, depth()}, {1, 1}, m_pmFillColor};
		v[3] = v[0];
		v[4] = v[2];
		v[5] = {{x0, y1, depth()}, {0, 1}, m_pmFillColor};
		m_vertexBuffer->unlock();
	}
	if (outlineColor().w > 0.0f) {
		auto pcolor = m_pmFillColor;
		addLine(x0, y0, x1, y0);
		addLine(x1, y0, x1, y1);
		addLine(x1, y1, x0, y1);
		addLine(x0, y1, x0, y0);
		m_pmFillColor = pcolor;
	}
}

void DrawList::addLine(float x0, float y0, float x1, float y1) {

	if (m_pmFillColor.w <= 0.0f) return;

	Vec3f p0{x0, y0, depth()};
	Vec3f p1{x1, y1, depth()};
	Vec3f tv{y0 - y1, x1 - x0, 0};
	tv = normalize(tv) * (lineWidth() * 0.5f);

	if (lineSmoothing()) {
		Vec4f blk(0);
		auto v = allocTriangles(4);
		v[0] = {p0, {0, 0}, m_pmFillColor};
		v[1] = {p1, {0, 0}, m_pmFillColor};
		v[2] = {p1 + tv, {0, 0}, blk};
		v[3] = v[0];
		v[4] = v[2];
		v[5] = {p0 + tv, {0, 0}, blk};
		//
		v[6] = {p0 - tv, {0, 0}, blk};
		v[7] = {p1 - tv, {0, 0}, blk};
		v[8] = {p1, {0, 0}, m_pmFillColor};
		v[9] = v[6];
		v[10] = v[8];
		v[11] = {p0, {0, 0}, m_pmFillColor};
		m_vertexBuffer->unlock();
	}
}

void DrawList::addOval(float minX, float minY, float maxX, float maxY) {

	float xr = (maxX - minX) / 2, x = minX + xr;
	float yr = (maxY - minY) / 2, y = minY + yr;

	float d = length(Vec2f(xr, yr));
	int n = std::max(int(d * 2), 12) & ~3;

	if (m_pmFillColor.w > 0.0f) {
		auto v = allocTriangles(n - 2);

		float th0 = (0.5f) * twoPi / n;
		float x0 = x + std::cos(th0) * xr;
		float y0 = y + std::sin(th0) * yr;

		float th1 = (1.5f) * twoPi / n;
		float x1 = x + std::cos(th1) * xr;
		float y1 = y + std::sin(th1) * yr;

		for (int i = 2; i < n; ++i) {

			float th2 = (i + 0.5f) * twoPi / n;
			float x2 = x + std::cos(th2) * xr;
			float y2 = y + std::sin(th2) * yr;

			v[0] = {{x0, y0, depth()}, {0, 0}, m_pmFillColor};
			v[1] = {{x1, y1, depth()}, {0, 0}, m_pmFillColor};
			v[2] = {{x2, y2, depth()}, {0, 0}, m_pmFillColor};

			x1 = x2;
			y1 = y2;
			v += 3;
		}
		m_vertexBuffer->unlock();
	}
	if (m_pmOutlineColor.w > 0.0f) {
		float th = (0.5f) * twoPi / n;
		float px1 = x + std::cos(th) * xr;
		float py1 = y + std::sin(th) * yr;

		for (int i = 0; i < n; ++i) {
			float px0 = px1;
			float py0 = py1;
			float th = (i + 1.5f) * twoPi / n;

			px1 = x + std::cos(th) * xr;
			py1 = y + std::sin(th) * yr;

			addLine(px0, py0, px1, py1);
		}
	}
}

void DrawList::addText(CString text, float x, float y) {

	if (m_pmTextColor.w <= 0.0f) return;

	auto pmaterial = fillMaterial();
	fillMaterial = font()->atlas;

	Vertex* v = allocTriangles(text.size() * 2);

	Vec2f pos(x, y + font()->baseline);

	for (uint8_t ch : text) {

		if (ch < Font::firstChar || ch >= font()->glyphs.size()) ch = Font::firstChar;

		CGlyph glyph = font()->glyphs[ch - Font::firstChar];

		CRectf src = glyph.srcRect;
		Rectf dst = glyph.dstRect + pos;

		pos.x += glyph.advance;

		v[0] = {{dst.min.x, dst.min.y, depth()}, {src.min.x, src.min.y}, m_pmTextColor};
		v[1] = {{dst.max.x, dst.min.y, depth()}, {src.max.x, src.min.y}, m_pmTextColor};
		v[2] = {{dst.max.x, dst.max.y, depth()}, {src.max.x, src.max.y}, m_pmTextColor};
		v[3] = v[0];
		v[4] = v[2];
		v[5] = {{dst.min.x, dst.max.y, depth()}, {src.min.x, src.max.y}, m_pmTextColor};

		v += 6;
	}
	m_vertexBuffer->unlock();

	fillMaterial = pmaterial;
}

void DrawList::onValidate(GraphicsContext* gc) const {

	m_renderOps = {};
	if(m_drawOps.empty()) return;

	RenderOp renderOp;
	renderOp.rendererBindings = m_bindGroup->wgpuBindGroup();
	renderOp.vertexBuffer = m_vertexBuffer->wgpuBuffer();
	renderOp.instanceCount = 1;
	for (auto& drawOp : m_drawOps) {
		auto cmaterial = drawOp.material ? drawOp.material : m_defaultMaterial;
		cmaterial->validate(gc);
		renderOp.pipeline = getOrCreateRenderPipeline(gc, cmaterial, m_bindGroup, DrawMode::triangleList);
		renderOp.materialBindings = cmaterial->bindGroup()->wgpuBindGroup();
		renderOp.elementCount = drawOp.triangleCount * 3;
		auto& renderOps = m_renderOps[(int)renderPassType(cmaterial->blendMode())];
		renderOps.push_back(renderOp);
		renderOp.firstElement += renderOp.elementCount;
	}
}

void DrawList::render(RenderContext* rc) const {
	Super::render(rc);
}

} // namespace sgd
