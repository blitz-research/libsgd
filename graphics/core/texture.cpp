#include "textureutil.h"

#include "dawn/generatemipmaps.h"

namespace sgd {

namespace {

wgpu::TextureFormat getFormat(TextureFormat format) {
	static const Map<TextureFormat, wgpu::TextureFormat> //
		formats{{TextureFormat::r8, wgpu::TextureFormat::R8Unorm},
				{TextureFormat::rg8, wgpu::TextureFormat::RG8Unorm},
				{TextureFormat::rgba8, wgpu::TextureFormat::RGBA8Unorm},
				{TextureFormat::srgba8, wgpu::TextureFormat::RGBA8UnormSrgb},
				//
				{TextureFormat::r8s, wgpu::TextureFormat::R8Snorm},
				{TextureFormat::rg8s, wgpu::TextureFormat::RG8Snorm},
				{TextureFormat::rgba8s, wgpu::TextureFormat::RGBA8Snorm},
				//
				{TextureFormat::r16, wgpu::TextureFormat::R16Unorm},
				{TextureFormat::rg16, wgpu::TextureFormat::RG16Unorm},
				{TextureFormat::rgba16, wgpu::TextureFormat::RGBA16Unorm},
				//
				{TextureFormat::r16s, wgpu::TextureFormat::R16Snorm},
				{TextureFormat::rg16s, wgpu::TextureFormat::RG16Snorm},
				{TextureFormat::rgba16s, wgpu::TextureFormat::RGBA16Snorm},
				//
				{TextureFormat::r16f, wgpu::TextureFormat::R16Float},
				{TextureFormat::rg16f, wgpu::TextureFormat::RG16Float},
				{TextureFormat::rgba16f, wgpu::TextureFormat::RGBA16Float},
				//
				{TextureFormat::r32f, wgpu::TextureFormat::R32Float},
				{TextureFormat::rg32f, wgpu::TextureFormat::RG32Float},
				{TextureFormat::rgba32f, wgpu::TextureFormat::RGBA32Float},
				//
				{TextureFormat::depth32f, wgpu::TextureFormat::Depth32Float}};

	auto it = formats.find(format);
	if (it != formats.end()) return it->second;
	SGD_ABORT();
}

TextureFormat getFormat(wgpu::TextureFormat format) {
	static const Map<wgpu::TextureFormat, TextureFormat> //
		formats{{wgpu::TextureFormat::R8Unorm, TextureFormat::r8},
				{wgpu::TextureFormat::RG8Unorm, TextureFormat::rg8},
				{wgpu::TextureFormat::RGBA8Unorm, TextureFormat::rgba8},
				{wgpu::TextureFormat::RGBA8UnormSrgb, TextureFormat::srgba8},
				//
				{wgpu::TextureFormat::R8Snorm, TextureFormat::rg8s},
				{wgpu::TextureFormat::RG8Snorm, TextureFormat::rgba8s},
				{wgpu::TextureFormat::RGBA8Snorm, TextureFormat::rgba8s},
				//
				{wgpu::TextureFormat::R16Unorm, TextureFormat::r16},
				{wgpu::TextureFormat::RG16Unorm, TextureFormat::rg16},
				{wgpu::TextureFormat::RGBA16Unorm, TextureFormat::rgba16},
				//
				{wgpu::TextureFormat::R16Snorm, TextureFormat::r16s},
				{wgpu::TextureFormat::RG16Snorm, TextureFormat::rg16s},
				{wgpu::TextureFormat::RGBA16Snorm, TextureFormat::rgba16s},
				//
				{wgpu::TextureFormat::R16Float, TextureFormat::r16f},
				{wgpu::TextureFormat::RG16Float, TextureFormat::rg16f},
				{wgpu::TextureFormat::RGBA16Float, TextureFormat::rgba16f},
				//
				{wgpu::TextureFormat::R32Float, TextureFormat::r32f},
				{wgpu::TextureFormat::RG32Float, TextureFormat::rg32f},
				{wgpu::TextureFormat::RGBA32Float, TextureFormat::rgba32f},
				//
				{wgpu::TextureFormat::Depth32Float, TextureFormat::depth32f}};

	auto it = formats.find(format);
	if (it != formats.end()) return it->second;
	SGD_ABORT();
}

wgpu::Sampler getOrCreateWGPUSampler(GraphicsContext* gc, TextureFlags flags) {

	static Map<TextureFlags, wgpu::Sampler> cache;

	auto key = flags & (TextureFlags::clamp | TextureFlags::filter | TextureFlags::mipmap | TextureFlags::compare);

	auto& sampler = cache[key];
	if (sampler) return sampler;

	wgpu::SamplerDescriptor desc{};
	desc.addressModeU = bool(flags & TextureFlags::clampU) ? wgpu::AddressMode::ClampToEdge : wgpu::AddressMode::Repeat;
	desc.addressModeV = bool(flags & TextureFlags::clampV) ? wgpu::AddressMode::ClampToEdge : wgpu::AddressMode::Repeat;
	desc.addressModeW = bool(flags & TextureFlags::clampW) ? wgpu::AddressMode::ClampToEdge : wgpu::AddressMode::Repeat;
	desc.magFilter = bool(flags & TextureFlags::filter) ? wgpu::FilterMode::Linear : wgpu::FilterMode::Nearest;
	desc.minFilter = wgpu::FilterMode::Linear;
	desc.mipmapFilter = bool(flags & TextureFlags::mipmap) ? wgpu::MipmapFilterMode::Linear : wgpu::MipmapFilterMode::Nearest;
	desc.compare = bool(flags & TextureFlags::compare) ? wgpu::CompareFunction::LessEqual : wgpu::CompareFunction::Undefined;
	desc.maxAnisotropy = bool(flags & TextureFlags::filter) && bool(flags & TextureFlags::mipmap) ? 16 : 1;

	return sampler = gc->wgpuDevice().CreateSampler(&desc);
}

} // namespace

Texture::Texture(TextureType type, CVec2u size, uint32_t depth, TextureFormat format, TextureFlags flags, TextureData* data)
	: m_type(type),		//
	  m_size(size),		//
	  m_depth(depth),	//
	  m_format(format), //
	  m_flags(flags),	//
	  m_data(data) {
	SGD_ASSERT(size.y * depth == data->size().y);
	SGD_ASSERT(format == data->format());
	m_dirty = true;
}

Texture::Texture(TextureType type, CVec2u size, uint32_t depth, TextureFormat format, TextureFlags flags)
	: m_type(type),		//
	  m_size(size),		//
	  m_depth(depth),	//
	  m_format(format), //
	  m_flags(flags),
	  m_data(!bool(flags & TextureFlags::renderTarget) ? new TextureData({size.x, size.y * depth}, format) : nullptr) {
}

Texture::Texture(Texture* texture, uint32_t layer)
	: m_type(TextureType::e2d),			//
	  m_size(texture->size()),			//
	  m_depth(1),						//
	  m_format(texture->format()),		//
	  m_flags(TextureFlags::layerView), //
	  m_texture(texture),				//
	  m_layer(layer) {
	addDependency(m_texture);
}

void Texture::update(const void* src, size_t pitch) {
	if (m_data->pitch() != pitch) SGD_ERROR("TODO");

	std::memcpy(m_data->data(), src, m_data->pitch() * m_data->size().y);

	m_dirty = true;
	invalidate();
}

void Texture::updateLayerView() const {
	m_wgpuTexture = m_texture->m_wgpuTexture;
	m_wgpuSampler = m_texture->m_wgpuSampler;
	wgpu::TextureViewDescriptor tvDesc{};
	tvDesc.format = m_wgpuTexture.GetFormat();
	tvDesc.dimension = wgpu::TextureViewDimension::e2D;
	tvDesc.baseArrayLayer = m_layer;
	tvDesc.arrayLayerCount = 1;
	m_wgpuTextureView = m_wgpuTexture.CreateView(&tvDesc);
}

void Texture::updateTexture() const {
	auto gc = currentGC();

	wgpu::TextureDescriptor desc{};
	desc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
	desc.size = {m_size.x, m_size.y, m_depth};
	desc.format = getFormat(m_format);
	if (bool(m_flags & TextureFlags::mipmap)) {
		desc.mipLevelCount = (uint32_t)std::floor(log2(std::max(desc.size.width, desc.size.height))) + 1u;
		desc.usage |= wgpu::TextureUsage::RenderAttachment;
	}
	if (bool(m_flags & TextureFlags::renderTarget)) {
		desc.usage |= wgpu::TextureUsage::RenderAttachment;
		if (bool(m_flags & TextureFlags::msaa)) desc.sampleCount = 4;
	}

	m_wgpuTexture = gc->wgpuDevice().CreateTexture(&desc);

	m_wgpuSampler = getOrCreateWGPUSampler(gc, m_flags);

	wgpu::TextureViewDescriptor tvDesc{};
	tvDesc.arrayLayerCount = m_depth;
	tvDesc.format = m_wgpuTexture.GetFormat();

	switch(m_type) {
	case TextureType::e2d:
		tvDesc.dimension = wgpu::TextureViewDimension::e2D;
		break;
	case TextureType::array:
		tvDesc.dimension = wgpu::TextureViewDimension::e2DArray;
		break;
	case TextureType::cube:
		tvDesc.dimension = wgpu::TextureViewDimension::Cube;
		break;
	case TextureType::cubeArray:
		tvDesc.dimension = wgpu::TextureViewDimension::CubeArray;
		break;
	}

	m_wgpuTextureView = m_wgpuTexture.CreateView(&tvDesc);
}

void Texture::updateData() const {
	auto gc = currentGC();

	wgpu::ImageCopyTexture dst{};
	dst.texture = m_wgpuTexture;

	wgpu::TextureDataLayout layout{};
	layout.bytesPerRow = m_data->pitch();
	layout.rowsPerImage = m_size.y;

	wgpu::Extent3D extent{m_size.x, m_size.y, m_depth};

	gc->wgpuDevice().GetQueue().WriteTexture(&dst, m_data->data(), layout.bytesPerRow * layout.rowsPerImage * m_depth, &layout,
											 &extent);

	if (m_wgpuTexture.GetMipLevelCount() > 1) generateMipmaps(gc->wgpuDevice(), m_wgpuTexture);
}

void Texture::onValidate() const {
	auto gc = currentGC();

	if (m_flags == TextureFlags::layerView) {
		updateLayerView();
		return;
	}

	if (!m_wgpuTexture) {
		updateTexture();
	}

	if (m_dirty) {
		SGD_ASSERT(m_data);
		m_dirty = false;
		updateData();
	}
}

} // namespace sgd
