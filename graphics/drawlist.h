#pragma once

#include "renderer.h"

#include "font.h"

namespace sgd {

SGD_SHARED(DrawList);

struct DrawList : public Renderer {
	SGD_OBJECT_TYPE(DrawList, Renderer);

	struct Vertex {
		Vec3f position;
		Vec2f texCoords;
		Vec4f color;
	};
	using CVertex = const Vertex&;

	Property<Mat4f> projectionMatrix;

	Property<Vec4f> fillColor{Vec4f(0, 0, 0, 1)};
	Property<CMaterialPtr> fillMaterial{};
	Property<bool> fillEnabled{true};

	Property<Vec4f> outlineColor{Vec4f(1)};
	Property<float> outlineWidth{1};
	Property<bool> outlineEnabled{false};

	Property<bool> lineSmoothing{true};
	Property<float> lineWidth{2};

	Property<float> pointSize{2};

	Property<CFontPtr> font;
	Property<Vec4f> textColor{Vec4f(1)};

	Property<float> depth{0};

	DrawList();

	void clear();
	void flush();

	void addOutline(CVec2f v0, CVec2f v1);
	void addLine(CVec2f v0, CVec2f v1);
	void addRect(CRectf rect);
	void addOval(CRectf rect);
	void addPoint(CVec2f p);
	void addText(CString text, CVec2f v);

private:
	struct DrawOp {
		CMaterialPtr material;
		uint32_t triangleCount;
	};

	BindGroupPtr m_bindGroup;
	BufferPtr m_uniformBuffer;
	uint32_t m_vertexCapacity{1000};
	uint32_t m_vertexCount{};
	BufferPtr m_vertexBuffer;

	Vec4f m_pmFillColor;
	Vec4f m_pmOutlineColor;
	Vec4f m_pmTextColor;

	Vector<DrawOp> m_drawOps;
	DrawOp m_drawOp;

	CMaterialPtr m_defaultMaterial;

	Vertex* allocTriangles(uint32_t count);

#if 0
	struct DrawOp {
		MaterialPtr material;
		uint32_t count;
	};

	Vector<DrawOp> m_drawOps;
	DrawOp m_drawOp;

	Vertex* addDrawOp(Material* material, uint32_t order, uint32_t count);
#endif

	void onValidate(GraphicsContext* gc) const override;
	void render(RenderContext* rc) const override;
};

} // namespace sgd
