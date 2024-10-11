#pragma once

#include "texturedata.h"

#include "graphics.h"

//! @file

namespace sgd {

enum struct TextureType {
	e2d = 1,
	cube = 2,
	array = 3,
	cubeArray = 4,
};

//! TextureFlags enum
enum struct TextureFlags {
	none = 0x00,
	clampU = 0x01,
	clampV = 0x02,
	clampW = 0x04,
	clamp = 0x07,
	filter = 0x08,
	mipmap = 0x10,
	default_ = mipmap | filter,
	image = default_ | clamp,
	allMask = 0x1f,
	//
	// TODO: Move to texture type enum.
	//
//	cube = 0x00100,
//	array = 0x00200,
	renderTarget = 0x00400,
	layerView = 0x00800,
	msaa = 0x01000,
	compare = 0x02000,
};

SGD_SHARED(Texture);

struct Texture : GraphicsResource {
	SGD_OBJECT_TYPE(Texture, Shared);

	Texture(TextureType type, CVec2u size, uint32_t depth, TextureFormat format, TextureFlags flags, TextureData* data);
	Texture(TextureType type, CVec2u size, uint32_t depth, TextureFormat format, TextureFlags flags);
	Texture(Texture* texture, uint32_t layer);

	Property<Path> path;

	TextureType type() const {
		return m_type;
	}

	CVec2u size() const {
		return m_size;
	}

	TextureFormat format() const {
		return m_format;
	}

	uint32_t depth() const {
		return m_depth;
	}

	TextureFlags flags() const {
		return m_flags;
	}

	CTextureData* data() const {
		return m_data;
	}

	TextureData* lock() {
		return m_data;
	}

	void unlock() {
		if (m_dirty) return;
		m_dirty = true;
		invalidate();
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
	TextureType m_type;
	Vec2u m_size;
	uint32_t m_depth;
	TextureFormat m_format;
	TextureFlags m_flags;

	TextureDataPtr m_data{};
	TexturePtr m_texture{};
	uint32_t m_layer{};

	// TODO: Should be a rect
	mutable bool m_dirty{};

	mutable wgpu::Texture m_wgpuTexture;
	mutable wgpu::Sampler m_wgpuSampler;
	mutable wgpu::TextureView m_wgpuTextureView;

	void updateLayerView() const;
	void updateTexture() const;
	void updateData() const;

	void onValidate() const override;
};

} // namespace sgd
