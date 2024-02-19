#pragma once

#include <geom/exports.h>

#include <dawn/native/DawnNative.h>
#include <dawn/webgpu_cpp.h>

namespace sgd {

struct XRView {
	AffineMat4f transform;
	Mat4f projection;
	Recti viewport;
	float clipNear;
	float clipFar;
};

struct XRViewerPose {
	AffineMat4f transform;
	XRView views[2];
};

struct XRHandPose {
	AffineMat4f transform;
};

struct XRFrame {
	wgpu::Texture swapChainImage;
	XRViewerPose viewerPose;
	Array<XRHandPose, 2> handPoses;
};

struct XRSession {

	virtual const wgpu::Device& wgpuDevice() const = 0;
	virtual wgpu::TextureFormat swapChainFormat() const = 0;
	virtual Vec2u swapChainSize() const = 0;
	virtual const XRFrame& beginFrame() = 0;
	virtual void endFrame() = 0;
};

wgpu::Device createXRCompatibleWGPUDevice();

XRSession* createXRSession(const wgpu::Device& wgpuDevice);

} // namespace sgd
