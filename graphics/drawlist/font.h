#pragma once

#include "../material/material.h"

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
	static constexpr Vec2u atlasSize = {1024,1024};

	CMaterialPtr const atlas;
	Vector<Glyph> const glyphs;
	float const height;
	float const ascent;
	float const descent;
	float const lineGap;

	Font(CMaterial* atlas, Vector<Glyph> glyphs, float height, float ascent, float descent, float lineGap)
		: atlas(atlas), glyphs(std::move(glyphs)), height(height), ascent(ascent), descent(descent), lineGap(lineGap) {
	}

	const Glyph& glyph(uint32_t chr) const {
		chr-=firstChar;
		return chr<glyphs.size() ? glyphs[chr] : glyphs[0];
	}

	float textWidth(CString text) const;
};

Expected<Font*, FileioEx> loadFont(CPath path, float height);

Font* defaultFont();

} // namespace sgd
