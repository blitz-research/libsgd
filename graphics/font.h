#pragma once

#include "material.h"

namespace sgd {

struct Glyph {
	Rectf const srcRect; // Source rect in normalized tex coordinates
	Rectf const dstRect; // Destination rect in texels
	float const advance;

	Glyph(CRectf srcRect, CRectf dstRect, float advance) : srcRect(srcRect), dstRect(dstRect), advance(advance) {
	}
};
using CGlyph = const Glyph&;

SGD_SHARED(Font);

class Font : public Shared {
public:
	SGD_OBJECT_TYPE(Font, Shared);

	static constexpr uint32_t firstChar = 32;
	static constexpr uint32_t charCount = 96;
	static constexpr Vec2u atlasSize = {512, 512};

	CMaterialPtr const atlas;
	Vector<Glyph> const glyphs;
	float const height;
	float const ascent;
	float const descent;
	float const lineGap;
	float const baseline;

	Font(CMaterial* atlas, Vector<Glyph> glyphs, float height, float ascent, float descent, float lineGap, float baseline)
		: atlas(atlas), glyphs(std::move(glyphs)), height(height), ascent(ascent), descent(descent), lineGap(lineGap),
		  baseline(baseline) {
	}

	float textWidth(CString text);
};

Expected<Font*, FileioEx> loadFont(CPath path, float height);

} // namespace sgd
