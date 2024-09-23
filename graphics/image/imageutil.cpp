#include "imageutil.h"

#include "../core/textureutil.h"

#include "imagematerial.h"

namespace sgd {

Expected<Image*, FileioEx> loadImage(CPath path, uint32_t depth) {

	auto flags = TextureFlags::array | TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clamp;

	auto texture = loadArrayTexture(path, TextureFormat::any, flags, depth);
	if (!texture) return texture.error();

	return new Image(texture.result());
}

Expected<Image*, FileioEx> loadImageEx(CPath path, uint32_t frameCount, uint32_t framesX, uint32_t framesY,
									   uint32_t frameSpacing) {
	if (!frameCount || !framesX || !framesY) SGD_ERROR("Invalid frame layout parameters");

	auto format = TextureFormat::any;

	auto rdata = loadTextureData(path, format);
	if (!rdata.result()) return rdata.error();

	auto srcData = rdata.result();
	auto size = srcData->size();
	auto frameSize = Vec2u((size.x - (framesX - 1) * frameSpacing) / framesX, //
						   (size.y - (framesY - 1) * frameSpacing) / framesY);

	if(!frameSize.x || !frameSize.y) SGD_ERROR("Invalid frame layout parameters");

	Vector<Vec2u> orgs;
	Vec2u pos;
	for (auto i = 0; i < frameCount; ++i) {
		orgs.push_back(pos);
		pos.x += frameSize.x + frameSpacing;
		if (pos.x + frameSize.x > size.x) {
			pos.x = 0;
			pos.y += frameSize.y + frameSpacing;
		}
	}

	auto flags = TextureFlags::array | TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clamp;
	auto texture = new Texture(frameSize, frameCount, srcData->format(), flags);

	auto texData = texture->lock();

	auto dst = texData->data();
	auto bpr = size.x * texData->bpp();

	for (auto i = 0; i < frameCount; ++i) {
		auto src = srcData->data() + orgs[i].y * srcData->pitch() + orgs[i].x * srcData->bpp();
		for (auto y = 0u; y < frameSize.y; ++y) {
			std::memcpy(dst, src, bpr);
			dst += texData->pitch();
			src += srcData->pitch();
		}
	}
	texture->unlock();

	return new Image(texture);
}

} // namespace sgd
