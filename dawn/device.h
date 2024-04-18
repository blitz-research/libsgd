#pragma once

#include <geom/exports.h>

#include <webgpu/webgpu_cpp.h>

struct GLFWwindow;

namespace sgd {

#if SGD_OS_WINDOWS
constexpr auto defaultBackendType = wgpu::BackendType::D3D12;
#elif SGD_OS_LINUX
constexpr auto defaultBackendType = wgpu::BackendType::Vulkan;
#elif SGD_OS_MACOS
constexpr auto defaultBackendType = wgpu::BackendType::Metal;
#elif SGD_OS_EMSCRIPTEN
constexpr auto defaultBackendType = wgpu::BackendType::WebGPU;
#endif

inline Signal<wgpu::Device> wgpuDeviceLost;

void requestWGPUDevice(const wgpu::RequestAdapterOptions& adapterOptions, CFunction<void(const wgpu::Device& device)> callback);

wgpu::Device createWGPUDevice(const wgpu::RequestAdapterOptions& adapterOptions);

wgpu::TextureFormat preferredWGPUSwapChainFormat(const wgpu::Device& device);

wgpu::Surface createWGPUSurface(const wgpu::Device& device, GLFWwindow* window);

wgpu::SwapChain createWGPUSwapChain(const wgpu::Device& device, const wgpu::Surface& surface, CVec2u size,
									wgpu::TextureFormat format);

void requestRender(CFunction<void()> renderFunc);

}
