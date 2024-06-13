#pragma once

#include "graphics.h"

//! @file

namespace sgd {

//! TextureFormat enum
enum struct TextureFormat {
	undefined = 0,
	r8,
	rg8,
	rgba8,
	srgba8,
	rgba16f,
	depth32f,
};

inline uint32_t bytesPerTexel(TextureFormat format) {
	uint32_t bpp[]{0, 1, 2, 4, 4, 16, 4};
	return bpp[(int)format];
}

//! TextureFlags enum
enum struct TextureFlags {
	none = 0,
	//
	clampU = 0x01,
	clampV = 0x02,
	clampW = 0x04,
	clamp = 0x07,
	filter = 0x08,
	mipmap = 0x10,
	cube = 0x20,
	array = 0x40,
	renderTarget = 0x80,
	layerView = 0x100,
	msaa = 0x200,
	compare = 0x400,

	envmapDefault = 0x38,
	materialDefault = 0x18,
	imageDefault = 0x1f,
};

SGD_SHARED(Texture);

struct Texture : GraphicsResource {
	SGD_OBJECT_TYPE(Texture, Shared);

	Texture(CVec2u size, uint32_t depth, TextureFormat format, TextureFlags flags);
	Texture(Texture* texture, uint32_t layer);
	~Texture() override;

	CVec2u size() const {
		return m_size;
	}

	uint32_t depth() const {
		return m_depth;
	}

	TextureFormat format() const {
		return m_format;
	}

	TextureFlags flags() const {
		return m_flags;
	}

	void update(const void* data, size_t pitch);

	const wgpu::Texture& wgpuTexture() const {
		SGD_ASSERT(valid());
		return m_wgpuTexture;
	};

	const wgpu::Sampler& wgpuSampler() const {
		SGD_ASSERT(valid());
		return m_wgpuSampler;
	};

	const wgpu::TextureView& wgpuTextureView() const {
		SGD_ASSERT(valid());
		return m_wgpuTextureView;
	};

private:
	Vec2u m_size;
	uint32_t m_depth;
	TextureFormat m_format;
	TextureFlags m_flags;
	TexturePtr m_texture;
	uint32_t m_layer;
	uint8_t* m_data;

	wgpu::TextureViewDescriptor m_viewDesc;

	// TODO: Should be a rect
	mutable bool m_dirty{};

	mutable wgpu::Texture m_wgpuTexture;
	mutable wgpu::Sampler m_wgpuSampler;
	mutable wgpu::TextureView m_wgpuTextureView;

	void onValidate(GraphicsContext* gc) const override;
};

} // namespace sgd
