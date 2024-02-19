#pragma once

#include "graphics.h"

//! @file

namespace sgd {

//! TextureFormat enum
enum struct TextureFormat {
	rgba8 = 0,
	srgba8 = 1,
	rgba16f = 2,
	depth32f = 3,
};

//! TextureFlags enum
enum struct TextureFlags {
	none = 0,
	//
	clampU = 0x01,
	clampV = 0x02,
	clampW = 0x04,
	//
	magFilter = 0x100,
	minFilter = 0x200,
	mipFilter = 0x400,
	//
	cube = 0x800,
	renderTarget = 0x1000,
	//
	clamp = clampU | clampV | clampW,
	filter = magFilter | minFilter | mipFilter
};

SGD_SHARED(Texture);

struct Texture : GraphicsResource {
	SGD_OBJECT_TYPE(Texture, Shared);

	Texture(CVec2u size, uint32_t depth, TextureFormat format, TextureFlags flags);
	~Texture();

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

	void resize(CVec2u size);

	void update(const void* data, size_t pitch);

	//! @private
	const wgpu::Texture& wgpuTexture() const {
		return m_wgpuTexture;
	};

	//! @private
	const wgpu::Sampler& wgpuSampler() const {
		return m_wgpuSampler;
	};

	//! @private
	const wgpu::TextureView& wgpuTextureView() const {
		return m_wgpuTextureView;
	};

private:
	Vec2u m_size;
	uint32_t m_depth;
	TextureFormat m_format;
	TextureFlags m_flags;
	uint8_t* m_data;

	// TODO: Should be a rect
	mutable bool m_dirty{};

	mutable wgpu::Texture m_wgpuTexture;
	mutable wgpu::Sampler m_wgpuSampler;
	mutable wgpu::TextureView m_wgpuTextureView;

	void onValidate(GraphicsContext* gc) const override;
};

} // namespace sgd
