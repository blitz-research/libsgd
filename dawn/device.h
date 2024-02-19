#pragma once

#include <geom/exports.h>

//#include <dawn/webgpu_cpp.h>

#include <webgpu/webgpu_cpp.h>

namespace sgd {

#if SGD_OS_WINDOWS
constexpr auto defaultBackendType = wgpu::BackendType::D3D12;
#else
constexpr auto defaultBackendType = wgpu::BackendType::Vulkan;
#endif

inline Signal<wgpu::Device> wgpuDeviceLost;

const wgpu::Instance& getWGPUInstance();

void requestWGPUDevice(const wgpu::RequestAdapterOptions& adapterOptions, CFunction<void(const wgpu::Device& device)> callback);

wgpu::Device createWGPUDevice(const wgpu::RequestAdapterOptions& adapterOptions);

wgpu::TextureFormat preferredWGPUSwapChainFormat(const wgpu::Device& device);

wgpu::Surface createWGPUSurface(const wgpu::Device& device, void* nativeWindow);

wgpu::SwapChain createWGPUSwapChain(const wgpu::Device& device, const wgpu::Surface& surface, CVec2u size, wgpu::TextureFormat format);

void requestRender(CFunction<void()> renderFunc);

}
