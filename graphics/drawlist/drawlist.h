#pragma once

#include "font.h"
#include "../image/image.h"

namespace sgd {

SGD_SHARED(RenderQueue);

SGD_SHARED(DrawList);

struct DrawList : public Shared {
	SGD_OBJECT_TYPE(DrawList, Shared);

	struct Vertex {
		Vec3f position;
		Vec3f texCoords;
		Vec4f color;
	};
	using CVertex = const Vertex&;

	Property<Mat4f> projectionMatrix;

	Property<Vec4f> fillColor{Vec4f(1, 1, 1, 1)};
	Property<CMaterialPtr> fillMaterial{};
	Property<bool> fillEnabled{true};

	Property<Vec4f> outlineColor{Vec4f(0, 0, 0, 1)};
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
	void addImage(CImage* image, CVec2f v, float frame);

	void pushLayer();
	void popLayer();

	void render(RenderQueue* rq) const;

private:
	struct DrawOp {
		CMaterialPtr material;
		uint32_t triangleCount;
	};

	BindGroupPtr m_bindGroup;
	BufferPtr m_uniformBuffer;
	uint32_t m_vertexCapacity{};
	uint32_t m_vertexCount{};
	BufferPtr m_vertexBuffer;

	Vec4f m_pmFillColor;
	Vec4f m_pmOutlineColor;
	Vec4f m_pmTextColor;

	Vector<DrawOp> m_drawOps;
	DrawOp m_drawOp;

	// Layers...
	Vector<uint32_t> m_drawOpCounts;
	Vector<uint32_t> m_vertexCounts;

	CMaterialPtr m_whiteMaterial;

	Vertex* allocTriangles(uint32_t count, CMaterial* material);
};

} // namespace sgd
