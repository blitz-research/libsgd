#pragma once

#include "device.h"

namespace sgd {

wgpu::ShaderModule createShaderModule(const wgpu::Device& device, CString source);

constexpr wgpu::BindGroupLayoutEntry bufferBindGroupLayoutEntry(uint32_t binding, wgpu::ShaderStage visibility,
																wgpu::BufferBindingType bindingType) {
	return wgpu::BindGroupLayoutEntry{nullptr, binding, visibility, {nullptr, bindingType}};
}

constexpr wgpu::BindGroupLayoutEntry textureBindGroupLayoutEntry(uint32_t binding, wgpu::ShaderStage visibility, wgpu::TextureViewDimension viewDimension=wgpu::TextureViewDimension::e2D) {
	return wgpu::BindGroupLayoutEntry{
		nullptr, binding, visibility, {}, {}, {nullptr, wgpu::TextureSampleType::Float, viewDimension}};
}

constexpr wgpu::BindGroupLayoutEntry samplerBindGroupLayoutEntry(uint32_t binding, wgpu::ShaderStage visibility) {
	return wgpu::BindGroupLayoutEntry{nullptr, binding, visibility, {}, {nullptr, wgpu::SamplerBindingType::Filtering}};
}

inline wgpu::BindGroupEntry bufferBindGroupEntry(uint32_t binding, wgpu::Buffer buffer) { //
	return wgpu::BindGroupEntry{nullptr, binding, buffer};
}

inline wgpu::BindGroupEntry textureBindGroupEntry(uint32_t binding, wgpu::TextureView textureView) { //
	return wgpu::BindGroupEntry{nullptr, binding, nullptr, 0, WGPU_WHOLE_SIZE, nullptr, textureView};
}

inline wgpu::BindGroupEntry samplerBindGroupEntry(uint32_t binding, wgpu::Sampler sampler) { //
	return wgpu::BindGroupEntry{nullptr, binding, {}, {}, {}, sampler};
}

}
