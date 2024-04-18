
#if !SGD_DYNAMIC
#error ImGui backend should only be used in dynalic libs
#endif

#include "internal.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_wgpu.h>

namespace {

struct ImGuiProcs {

	// globals
	ImGuiID (*ImHashData)(const void*, size_t, ImU32);

	// namespace ImGui members
	ImGuiContext* (*GetCurrentContext)();
	ImGuiViewport* (*GetMainViewport)();
	int (*GetMouseCursor)();
	ImGuiIO& (*GetIO)();
	void* (*MemAlloc)(size_t);
	void (*MemFree)(void*);

	// struct ImGuiIO members
	void (ImGuiIO::*AddKeyEvent)(enum ImGuiKey, bool);
	void (ImGuiIO::*AddKeyAnalogEvent)(enum ImGuiKey, bool, float);
	void (ImGuiIO::*AddMousePosEvent)(float, float);
	void (ImGuiIO::*AddMouseButtonEvent)(int, bool);
	void (ImGuiIO::*AddMouseWheelEvent)(float, float);
	void (ImGuiIO::*AddMouseSourceEvent)(enum ImGuiMouseSource);
	void (ImGuiIO::*AddFocusEvent)(bool);
	void (ImGuiIO::*AddInputCharacter)(unsigned int);
	void (ImGuiIO::*SetKeyEventNativeData)(enum ImGuiKey, int, int, int);

	// struct ImGuiStorage members
	void* (ImGuiStorage::*GetVoidPtr)(ImGuiID key) const;
	void (ImGuiStorage::*SetVoidPtr)(ImGuiID key, void *value);

	// Struct ImFontAtlas members
	void (ImFontAtlas::*GetTexDataAsRGBA32)(unsigned char**, int*, int*, int*);
};

ImGuiProcs g_procs{};

}

// ***** ImGui_ImplGlfw *****

SGD_Bool SGD_DECL sgd_ImGui_ImplSGD_Init(void* imguiProcs) {

	g_procs=*(ImGuiProcs*)imguiProcs;

	if(!ImGui_ImplGlfw_InitForOther(sgdx::mainWindow()->glfwWindow(), true)) {
		return false;
	}
#ifdef __EMSCRIPTEN__
	ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif

	ImGui_ImplWGPU_InitInfo init_info;
	init_info.Device = sgdx::mainGC()->wgpuDevice().Get();
	init_info.NumFramesInFlight = 3;
	init_info.RenderTargetFormat = WGPUTextureFormat_RGBA16Float;
	init_info.DepthStencilFormat = WGPUTextureFormat_Undefined;
	return ImGui_ImplWGPU_Init(&init_info);
}

void SGD_DECL sgd_ImGui_ImplSGD_Shutdown() {
	ImGui_ImplWGPU_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}

void SGD_DECL sgd_ImGui_ImplSGD_NewFrame() {
	ImGui_ImplWGPU_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}

SGD_API void SGD_DECL sgd_ImGui_ImplSGD_RenderDrawData(void* imguiDrawData) {

	if(!sgdx::mainGC()->canRender()) return;

	auto device = sgdx::mainGC()->wgpuDevice().Get();
	auto renderTarget = sgdx::mainGC()->colorBuffer()->wgpuTexture().Get();

	WGPURenderPassColorAttachment color_attachments = {};
	color_attachments.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
	color_attachments.loadOp = WGPULoadOp_Load;
	color_attachments.storeOp = WGPUStoreOp_Store;
	color_attachments.clearValue = {};
	color_attachments.view = wgpuTextureCreateView(renderTarget, nullptr);

	WGPURenderPassDescriptor render_pass_desc = {};
	render_pass_desc.colorAttachmentCount = 1;
	render_pass_desc.colorAttachments = &color_attachments;
	render_pass_desc.depthStencilAttachment = nullptr;

	WGPUCommandEncoderDescriptor enc_desc = {};
	WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &enc_desc);

	WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);
	ImGui_ImplWGPU_RenderDrawData((ImDrawData*)imguiDrawData, pass);
	wgpuRenderPassEncoderEnd(pass);

	WGPUCommandBufferDescriptor cmd_buffer_desc = {};
	WGPUCommandBuffer cmd_buffer = wgpuCommandEncoderFinish(encoder, &cmd_buffer_desc);
	WGPUQueue queue = wgpuDeviceGetQueue(device);
	wgpuQueueSubmit(queue, 1, &cmd_buffer);

	wgpuTextureViewRelease(color_attachments.view);
}

// ***** ImGui procs ****

ImGuiID ImHashData(const void* data_p, size_t data_size, ImU32 seed) {
	return g_procs.ImHashData(data_p, data_size, seed);
}

ImGuiContext* ImGui::GetCurrentContext() {
	return g_procs.GetCurrentContext();
}

ImGuiViewport* ImGui::GetMainViewport() {
	return g_procs.GetMainViewport();
}

int ImGui::GetMouseCursor() {
	return g_procs.GetMouseCursor();
}

void* ImGui::MemAlloc(size_t size) {
	return g_procs.MemAlloc(size);
}

void ImGui::MemFree(void* ptr) {
	return g_procs.MemFree(ptr);
}

ImGuiIO& ImGui::GetIO() {
	return g_procs.GetIO();
}

void ImGuiIO::AddKeyEvent(enum ImGuiKey key, bool bown) {
	return (this->*g_procs.AddKeyEvent)(key, bown);
}

void ImGuiIO::AddKeyAnalogEvent(enum ImGuiKey key, bool down, float v) {
	return (this->*g_procs.AddKeyAnalogEvent)(key, down, v);
}

void ImGuiIO::AddMousePosEvent(float x, float y) {
	return (this->*g_procs.AddMousePosEvent)(x, y);
}

void ImGuiIO::AddMouseButtonEvent(int id, bool down) {
	return (this->*g_procs.AddMouseButtonEvent)(id, down);
}

void ImGuiIO::AddMouseWheelEvent(float x, float y) {
	return (this->*g_procs.AddMouseWheelEvent)(x, y);
}

void ImGuiIO::AddMouseSourceEvent(enum ImGuiMouseSource src) {
	return (this->*g_procs.AddMouseSourceEvent)(src);
}

void ImGuiIO::AddFocusEvent(bool focus) {
	return (this->*g_procs.AddFocusEvent)(focus);
}

void ImGuiIO::AddInputCharacter(unsigned int chr) {
	return (this->*g_procs.AddInputCharacter)(chr);
}

void ImGuiIO::SetKeyEventNativeData(ImGuiKey key, int native_keycode, int native_scancode, int native_legacy_index) {
	return (this->*g_procs.SetKeyEventNativeData)(key, native_keycode, native_keycode, native_legacy_index);
}

void* ImGuiStorage::GetVoidPtr(ImGuiID key) const{
	return (this->*g_procs.GetVoidPtr)(key);
}

void ImGuiStorage::SetVoidPtr(ImGuiID key, void *value) {
	return (this->*g_procs.SetVoidPtr)(key, value);
}

void ImFontAtlas::GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel) {
	return (this->*g_procs.GetTexDataAsRGBA32)(out_pixels, out_width, out_height, out_bytes_per_pixel);
}
