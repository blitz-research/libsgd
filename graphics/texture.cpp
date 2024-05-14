#include "textureutil.h"

#include "dawn/generatemipmaps.h"

#define PITCH(SZ) ((size_t)(SZ.x) * bytesPerTexel(m_format))

#define SIZE(SZ) (PITCH(SZ) * (SZ).y * m_depth)

namespace sgd {

namespace {

wgpu::TextureFormat getFormat(TextureFormat format) {
	static const Map<TextureFormat, wgpu::TextureFormat> //
		formats{{TextureFormat::r8, wgpu::TextureFormat::R8Unorm},
				{TextureFormat::rg8, wgpu::TextureFormat::RG8Unorm},
				{TextureFormat::rgba8, wgpu::TextureFormat::RGBA8Unorm},
				{TextureFormat::srgba8, wgpu::TextureFormat::RGBA8UnormSrgb},
				{TextureFormat::rgba16f, wgpu::TextureFormat::RGBA16Float},
				{TextureFormat::depth32f, wgpu::TextureFormat::Depth32Float}};

	return formats.find(format)->second;
}

TextureFormat getFormat(wgpu::TextureFormat format) {
	static const Map<wgpu::TextureFormat, TextureFormat> //
		formats{{wgpu::TextureFormat::R8Unorm, TextureFormat::r8},
				{wgpu::TextureFormat::RG8Unorm, TextureFormat::rg8},
				{wgpu::TextureFormat::RGBA8Unorm, TextureFormat::rgba8},
				{wgpu::TextureFormat::RGBA8UnormSrgb, TextureFormat::srgba8},
				{wgpu::TextureFormat::RGBA16Float, TextureFormat::rgba16f},
				{wgpu::TextureFormat::Depth32Float, TextureFormat::depth32f}};

	return formats.find(format)->second;
}

wgpu::Sampler getOrCreateWGPUSampler(GraphicsContext* gc, TextureFlags flags) {

	static Map<TextureFlags, wgpu::Sampler> cache;

	auto& sampler = cache[flags & (TextureFlags::mipmap | TextureFlags::filter | TextureFlags::clamp)];
	if (sampler) return sampler;

	wgpu::SamplerDescriptor desc{};
	desc.addressModeU = bool(flags & TextureFlags::clampU) ? wgpu::AddressMode::ClampToEdge : wgpu::AddressMode::Repeat;
	desc.addressModeV = bool(flags & TextureFlags::clampV) ? wgpu::AddressMode::ClampToEdge : wgpu::AddressMode::Repeat;
	desc.addressModeW = bool(flags & TextureFlags::clampW) ? wgpu::AddressMode::ClampToEdge : wgpu::AddressMode::Repeat;

	desc.magFilter = bool(flags & TextureFlags::filter) ? wgpu::FilterMode::Linear : wgpu::FilterMode::Nearest;
	desc.minFilter = wgpu::FilterMode::Linear;
	desc.mipmapFilter = wgpu::MipmapFilterMode::Linear;

	return sampler = gc->wgpuDevice().CreateSampler(&desc);
}

} // namespace

Texture::Texture(CVec2u size, uint32_t depth, TextureFormat format, TextureFlags flags)
	: m_size(size), m_depth(depth), m_format(format), m_flags(flags) {

	m_data = (uint8_t*)std::malloc(SIZE(m_size));
}

Texture::Texture(Texture* texture, uint32_t layer)
	: m_size(texture->m_size), m_depth(1), m_format(texture->m_format), m_flags(TextureFlags::layerView), //
	  m_texture(texture), m_layer(layer), m_data(nullptr) {

	addDependency(m_texture);
}

Texture::~Texture() {

	std::free(m_data);
}

void Texture::update(const void* src, size_t srcPitch) {

	auto dstPitch = PITCH(m_size);

	if (dstPitch != srcPitch) {
		SGD_PANIC("TODO");
		auto rowSize = std::min(srcPitch, dstPitch);
		for (int y = 0; y < m_size.y * m_depth; ++y) {
			std::memcpy(m_data + dstPitch * y, (uint8_t*)src + srcPitch * y, rowSize);
		}
	} else {
		std::memcpy(m_data, src, dstPitch * m_size.y * m_depth);
	}

	m_dirty = true;
	invalidate();
}

void Texture::onValidate(GraphicsContext* gc) const {

	if (m_flags == TextureFlags::layerView) {
		m_wgpuTexture = m_texture->m_wgpuTexture;
		m_wgpuSampler = m_texture->m_wgpuSampler;
		wgpu::TextureViewDescriptor tvDesc{};
		tvDesc.format = m_wgpuTexture.GetFormat();
		tvDesc.dimension = wgpu::TextureViewDimension::e2D;
		tvDesc.baseArrayLayer = m_layer;
		tvDesc.arrayLayerCount = 1;
		m_wgpuTextureView = m_wgpuTexture.CreateView(&tvDesc);
		return;
	}

	if (!m_wgpuTexture) {

		// Create texture
		wgpu::TextureDescriptor desc{};
		desc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
		desc.size = {m_size.x, m_size.y, m_depth};
		desc.format = getFormat(m_format);
		if (bool(m_flags & TextureFlags::mipmap)) {
			desc.mipLevelCount = (uint32_t)std::floor(log2(std::max(m_size.x, m_size.y))) + 1u;
			desc.usage |= wgpu::TextureUsage::RenderAttachment;
		}
		if (bool(m_flags & TextureFlags::renderTarget)) {
			desc.usage |= wgpu::TextureUsage::RenderAttachment;
		}
		m_wgpuTexture = gc->wgpuDevice().CreateTexture(&desc);

		m_wgpuSampler = getOrCreateWGPUSampler(gc, m_flags);

		wgpu::TextureViewDescriptor tvDesc{};
		tvDesc.arrayLayerCount = m_depth;
		tvDesc.format = m_wgpuTexture.GetFormat();

		if (bool(m_flags & TextureFlags::array)) {
			if (bool(m_flags & TextureFlags::cube)) {
				tvDesc.dimension = wgpu::TextureViewDimension::CubeArray;
			} else {
				tvDesc.dimension = wgpu::TextureViewDimension::e2DArray;
			}
		} else if (bool(m_flags & TextureFlags::cube)) {
			tvDesc.dimension = wgpu::TextureViewDimension::Cube;
		} else {
			tvDesc.dimension = wgpu::TextureViewDimension::e2D;
		}

		m_wgpuTextureView = m_wgpuTexture.CreateView(&tvDesc);
	}

	if (m_dirty) {

		wgpu::ImageCopyTexture dst{};
		dst.texture = m_wgpuTexture;

		wgpu::TextureDataLayout layout{};
		layout.bytesPerRow = PITCH(m_size);
		layout.rowsPerImage = m_size.y;

		wgpu::Extent3D extent{m_size.x, m_size.y, m_depth};

		gc->wgpuDevice().GetQueue().WriteTexture(&dst, m_data, layout.bytesPerRow * layout.rowsPerImage * m_depth, &layout,
												 &extent);

		if (m_wgpuTexture.GetMipLevelCount() > 1) generateMipmaps(gc->wgpuDevice(), m_wgpuTexture);

		m_dirty = false;
	}
}

} // namespace sgd
