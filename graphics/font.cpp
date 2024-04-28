#include "font.h"

#include "shaders/uniforms.h"
#include "textureutil.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_RASTERIZER_VERSION 1
#include <stb_truetype.h>

namespace sgd {

namespace {

auto shaderSource{
#include "shaders/fontmaterial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	1,
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform), // PrelitMaterialUniforms
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment),								  // albedoTexture
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),								  // albedoSampler
	},
	{}, shaderSource);

const MaterialDescriptor fontMaterialDescriptor( //
	"font",										 //
	&bindGroupDescriptor,						 //
	sizeof(FontMaterialUniforms),
	{
		{"atlasColor4f", {offsetof(FontMaterialUniforms, atlasColor), 4, new Vec4f(1)}},
	},
	{
		{"atlasTexture", {1, whiteTexture()}},
	});
} // namespace

float Font::textWidth(CString text) {

	float w = 0;
	for (uint8_t ch : text) {
		if (ch < firstChar || ch >= glyphs.size()) ch = firstChar;
		w += glyphs[ch - firstChar].advance;
	}
	return w;
}

Expected<Font*, FileioEx> loadFont(CPath path, float height) {

	auto data = loadData(path);
	if (!data) return data.error();

	stbtt_fontinfo info;
	if (!stbtt_InitFont(&info, data.result().data(), 0)) {
		return FileioEx("Failed to initialize font \"" + path.str() + "\"");
	}

	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
	float fscale = height / (float)(ascent - descent);
	float baseline=0;

	Data atlasData(Font::atlasSize.x * Font::atlasSize.y);

	Vector<Glyph> glyphs;

	{
		Vector<stbtt_packedchar> charData(Font::charCount);
		stbtt_pack_context packer;

		stbtt_PackBegin(&packer, atlasData.data(), (int)Font::atlasSize.x, (int)Font::atlasSize.y, 0, 1, nullptr);

		stbtt_PackSetOversampling(&packer, 1, 1);

		stbtt_PackFontRange(&packer, data.result().data(), 0, height * 2, Font::firstChar, Font::charCount, charData.data());

		stbtt_PackEnd(&packer);

		Vec2f texScale = 1.0f / Vec2f(Font::atlasSize);

		for (int i = 0; i < Font::charCount; ++i) {

			auto& ch = charData[i];

			baseline = std::max(baseline, -ch.yoff);

			Rectf srcRect = {Vec2f(ch.x0, ch.y0) * texScale, Vec2f(ch.x1, ch.y1) * texScale};
			Rectf dstRect = {Vec2f(ch.xoff, ch.yoff), Vec2f(ch.xoff2, ch.yoff2)};

			glyphs.emplace_back(srcRect, dstRect, ch.xadvance);
		}
	}

	auto texture = new Texture(Font::atlasSize, 1, TextureFormat::r8, TextureFlags::mipmap | TextureFlags::filter);
	texture->update(atlasData.data(), Font::atlasSize.x);

	auto atlas = new Material(&fontMaterialDescriptor);
	atlas->setTexture("atlasTexture", texture);
	atlas->blendMode = BlendMode::alpha;
	atlas->depthFunc = DepthFunc::always;
	atlas->cullMode = CullMode::none;

	return new Font(atlas, std::move(glyphs), height, (float)ascent * fscale, (float)descent * fscale, (float)lineGap * fscale,
					baseline);
}

} // namespace sgd