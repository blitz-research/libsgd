#pragma once

#include <geom/exports.h>

#include <webgpu/webgpu_cpp.h>

struct GLFWwindow;

namespace sgd {

inline Signal<wgpu::Device> wgpuDeviceLost;

void requestWGPUDevice(const wgpu::RequestAdapterOptions& adapterOptions, CFunction<void(const wgpu::Device& device)> callback);

wgpu::Device createWGPUDevice(const wgpu::RequestAdapterOptions& adapterOptions);

wgpu::Surface createWGPUSurface(const wgpu::Device& device, GLFWwindow* window);

void requestRender(CFunction<void()> renderFunc);

}
