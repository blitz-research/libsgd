#include "font.h"

#include "../core/textureutil.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_RASTERIZER_VERSION 1
#include <stb_truetype.h>

namespace sgd {

namespace {

struct alignas(16) FontUniforms {
	Vec4f atlasColor{1};
};

auto shaderSource{
#include "fontmaterial.wgsl"
};

BindGroupDescriptor bindGroupDescriptor( //
	"fontMaterial", //
	BindGroupType::material, //
	{
		bufferBindGroupLayoutEntry(0, wgpu::ShaderStage::Fragment, wgpu::BufferBindingType::Uniform), // PrelitMaterialUniforms
		textureBindGroupLayoutEntry(1, wgpu::ShaderStage::Fragment),								  // albedoTexture
		samplerBindGroupLayoutEntry(2, wgpu::ShaderStage::Fragment),								  // albedoSampler
	},
	shaderSource);

const MaterialDescriptor fontMaterialDescriptor( //
	"font",										 //
	&bindGroupDescriptor,						 //
	sizeof(FontUniforms),
	{
		{"atlasColor", {offsetof(FontUniforms, atlasColor), 4, new Vec4f(1)}},
	},
	{
		{"atlas", {1, whiteTexture()}},
	},1);

} // namespace

float Font::textWidth(CString text) const {
	float w = 0;
	for (uint8_t ch : text) {
		if ((ch -= firstChar) >= glyphs.size()) ch = 0;
		w += glyphs[ch].advance;
	}
	return w;
}

Expected<Font*, FileioEx> loadFont(CPath path, float height) {

	auto data = loadData(path);
	if (!data) return data.error();

	stbtt_fontinfo info;
	if (!stbtt_InitFont(&info, data.result().data(), 0)) {
		return SGD_FILEIOEX("Failed to initialize font \"" + path.str() + "\"");
	}

	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

	//	auto fscale = height / (float)(ascent - descent);
	auto fscale = stbtt_ScaleForMappingEmToPixels(&info, height);

	Data atlasData(Font::atlasSize.x * Font::atlasSize.y);

	Vector<Glyph> glyphs;

	{
		Vector<stbtt_packedchar> charData(Font::charCount);
		stbtt_pack_context packer;

		stbtt_PackBegin(&packer, atlasData.data(), (int)Font::atlasSize.x, (int)Font::atlasSize.y, 0, 1, nullptr);

		stbtt_PackSetOversampling(&packer, 1, 1);
		stbtt_PackFontRange(&packer, data.result().data(), 0,  STBTT_POINT_SIZE(height), Font::firstChar, Font::charCount, charData.data());

		stbtt_PackEnd(&packer);

		Vec2f texScale = 1.0f / Vec2f(Font::atlasSize);

		for (int i = 0; i < Font::charCount; ++i) {

			auto& ch = charData[i];

			Rectf srcRect = {Vec2f(ch.x0, ch.y0) * texScale, Vec2f(ch.x1, ch.y1) * texScale};

			Rectf dstRect = {Vec2f(ch.xoff, ch.yoff), Vec2f(ch.xoff2, ch.yoff2)};

			glyphs.emplace_back(srcRect, dstRect, ch.xadvance);
		}
	}

	auto texture = new Texture(TextureType::e2d, Font::atlasSize, 1, TextureFormat::r8, TextureFlags::default_);
	texture->update(atlasData.data(), Font::atlasSize.x);

	auto atlas = new Material(&fontMaterialDescriptor);
	atlas->setTexture("atlas", texture);
	atlas->blendMode = BlendMode::alphaBlend;
	atlas->depthFunc = DepthFunc::always;
	atlas->cullMode = CullMode::none;

	return new Font{atlas, std::move(glyphs), height, (float)ascent * fscale, (float)descent * fscale, (float)lineGap * fscale};
}

Font* defaultFont() {
	static FontPtr font;
	if (font) return font;

#if SGD_OS_WINDOWS
	font = loadFont(sgd::Path("C:/windows/fonts/consola.ttf"), 16).result();
#elif SGD_OS_LINUX
	font = loadFont(sgd::Path("/usr/share/fonts/TTF/Inconsolata-Medium.ttf"), 16).result();
#elif SGD_OS_MACOS
	font = loadFont(sgd::Path("/Library/Fonts/Arial Unicode.ttf"), 16).result();
#endif
	return font;
}

} // namespace sgd
