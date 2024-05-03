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

	DrawListUniforms uniforms;
	m_uniformBuffer->update(&uniforms, 0, sizeof(uniforms));

	m_bindGroup->setBuffer(0, m_uniformBuffer);

	auto cmaterial = new Material(&prelitMaterialDescriptor);
	//	cmaterial->blendMode = BlendMode::alpha;
	//	cmaterial->depthFunc = DepthFunc::always;
	//	cmaterial->cullMode = CullMode::none;
	m_defaultMaterial = cmaterial;

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

	clear();
}

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
	m_drawOp.material = {};
	m_drawOp.triangleCount = 0;
	m_vertexCount = 0;
	invalidate();
}

void DrawList::flush() {
	if (m_drawOp.triangleCount) {
		m_drawOps.push_back(m_drawOp);
		invalidate();
	}
	m_drawOp.material = {};
	m_drawOp.triangleCount = 0;
}

void DrawList::addPoint(CVec2f v) {
	float hsz = pointSize() * .5f;
	addOval({v - hsz, v + hsz});
}

void DrawList::addRect(CRectf r) {
	if (fillEnabled()) {
		auto vp = allocTriangles(2);
		vp[0] = {{topLeft(r), depth()}, {0, 0}, m_pmFillColor};
		vp[1] = {{topRight(r), depth()}, {1, 0}, m_pmFillColor};
		vp[2] = {{bottomRight(r), depth()}, {1, 1}, m_pmFillColor};
		vp[3] = vp[0];
		vp[4] = vp[2];
		vp[5] = {{bottomLeft(r), depth()}, {0, 1}, m_pmFillColor};
		m_vertexBuffer->unlock();
	}
	if (outlineEnabled()) {
		addOutline(topLeft(r), topRight(r));
		addOutline(topRight(r), bottomRight(r));
		addOutline(bottomRight(r), bottomLeft(r));
		addOutline(bottomLeft(r), topLeft(r));
	}
}

void DrawList::addOutline(CVec2f v0, CVec2f v1) {

	Vec2f dv = normalize(v1 - v0) * outlineWidth() * .5f;
	Vec3f p0{v0 - dv, depth()};
	Vec3f p1{v1 + dv, depth()};
	Vec3f tv{-dv.y, dv.x, 0};

	// Vec3f p0{v0, depth()};
	// Vec3f p1{v1, depth()};
	// Vec3f tv{v0.y - v1.y, v1.x - v0.x, 0};
	// tv = normalize(tv) * (outlineWidth() * 0.5f);

	auto pmaterial = fillMaterial();
	fillMaterial = m_defaultMaterial;

	auto vp = allocTriangles(2);
	vp[0] = {p0 + tv, {0, 0}, m_pmOutlineColor};
	vp[1] = {p0 - tv, {0, 0}, m_pmOutlineColor};
	vp[2] = {p1 - tv, {0, 0}, m_pmOutlineColor};
	vp[3] = vp[0];
	vp[4] = vp[2];
	vp[5] = {p1 + tv, {0, 0}, m_pmOutlineColor};
	m_vertexBuffer->unlock();

	fillMaterial = pmaterial;
}

void DrawList::addLine(CVec2f v0, CVec2f v1) {

	Vec2f dv = normalize(v1 - v0) * lineWidth() * .5f;
	Vec3f p0{v0 - dv, depth()};
	Vec3f p1{v1 + dv, depth()};
	Vec3f tv{-dv.y, dv.x, 0};

	// Vec3f p0{v0, depth()};
	// Vec3f p1{v1, depth()};
	// Vec3f tv{v0.y - v1.y, v1.x - v0.x, 0};
	// tv = normalize(tv) * (lineWidth() * 0.5f);

	if (fillEnabled()) {
		if (lineSmoothing()) {
			Vec4f blk(0);
			auto vp = allocTriangles(4);
			vp[0] = {p0, {0, 0}, m_pmFillColor};
			vp[1] = {p1, {0, 0}, m_pmFillColor};
			vp[2] = {p1 + tv, {0, 0}, blk};
			vp[3] = vp[0];
			vp[4] = vp[2];
			vp[5] = {p0 + tv, {0, 0}, blk};
			//
			vp[6] = {p0 - tv, {0, 0}, blk};
			vp[7] = {p1 - tv, {0, 0}, blk};
			vp[8] = {p1, {0, 0}, m_pmFillColor};
			vp[9] = vp[6];
			vp[10] = vp[8];
			vp[11] = {p0, {0, 0}, m_pmFillColor};
			m_vertexBuffer->unlock();
		} else {
			auto vp = allocTriangles(2);
			vp[0] = {p0 + tv, {0, 0}, m_pmFillColor};
			vp[1] = {p0 - tv, {0, 0}, m_pmFillColor};
			vp[2] = {p1 - tv, {0, 0}, m_pmFillColor};
			vp[3] = vp[0];
			vp[4] = vp[2];
			vp[5] = {p1 + tv, {0, 0}, m_pmFillColor};
			m_vertexBuffer->unlock();
		}
	}
	if (outlineEnabled()) {
		auto tv2 = Vec2f(tv);
		addOutline(v0 + tv2, v0 - tv2);
		addOutline(v0 - tv2, v1 - tv2);
		addOutline(v1 - tv2, v1 + tv2);
		addOutline(v1 + tv2, v0 + tv2);
	}
}

void DrawList::addOval(CRectf rect) {

	auto v = center(rect);
	auto r = rect.size() * .5f;

	float d = length(r);
	int n = std::max(int(d * 2), 12) & ~3;

	constexpr float off = 0; //.5f;

	if (fillEnabled()) {

		auto vp = allocTriangles(n);

		float th0 = (off)*twoPi / n;
		Vec2f v0{v.x + std::cos(th0) * r.x, v.y + std::sin(th0) * r.y};

		for (int i = 0; i < n; ++i) {
			float th1 = (i + 1 + off) * twoPi / n;
			Vec2f v1{v.x + std::cos(th1) * r.x, v.y + std::sin(th1) * r.y};

			vp[0] = {{v0, depth()}, {0, 0}, m_pmFillColor};
			vp[1] = {{v1, depth()}, {0, 0}, m_pmFillColor};
			vp[2] = {{v, depth()}, {0, 0}, m_pmFillColor};

			v0 = v1;
			vp += 3;
		}
		m_vertexBuffer->unlock();
	}
	if (outlineEnabled()) {

		float th0 = (off)*twoPi / n;
		Vec2f v0{v.x + std::cos(th0) * r.x, v.y + std::sin(th0) * r.y};

		for (int i = 0; i < n; ++i) {
			float th1 = (i + 1 + off) * twoPi / n;
			Vec2f v1{v.x + std::cos(th1) * r.x, v.y + std::sin(th1) * r.y};

			addOutline(v0, v1);

			v0 = v1;
		}
	}
}

void DrawList::addText(CString text, CVec2f v) {

	auto pmaterial = fillMaterial();
	fillMaterial = font()->atlas;

	Vertex* vp = allocTriangles(text.size() * 2);

	Vec2f pos(v.x, v.y + font()->ascent);

	for (uint8_t ch : text) {

		if ((ch -= font()->firstChar) >= font()->glyphs.size()) ch = 0;

		CGlyph glyph = font()->glyphs[ch];

		CRectf src = glyph.srcRect;
		Rectf dst = glyph.dstRect + pos;

		pos.x += glyph.advance;

		vp[0] = {{dst.min.x, dst.min.y, depth()}, {src.min.x, src.min.y}, m_pmTextColor};
		vp[1] = {{dst.max.x, dst.min.y, depth()}, {src.max.x, src.min.y}, m_pmTextColor};
		vp[2] = {{dst.max.x, dst.max.y, depth()}, {src.max.x, src.max.y}, m_pmTextColor};
		vp[3] = vp[0];
		vp[4] = vp[2];
		vp[5] = {{dst.min.x, dst.max.y, depth()}, {src.min.x, src.max.y}, m_pmTextColor};

		vp += 6;
	}
	m_vertexBuffer->unlock();

	fillMaterial = pmaterial;
}

void DrawList::onValidate(GraphicsContext* gc) const {

	m_renderOps = {};
	if (m_drawOps.empty()) return;

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
