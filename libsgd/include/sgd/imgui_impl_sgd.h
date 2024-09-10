#pragma once

#include "imgui.h" // IMGUI_IMPL_API
#ifndef IMGUI_DISABLE

IMGUI_IMPL_API bool ImGui_ImplSGD_Init();
IMGUI_IMPL_API void ImGui_ImplSGD_Shutdown();
IMGUI_IMPL_API void ImGui_ImplSGD_NewFrame();
IMGUI_IMPL_API void ImGui_ImplWGPU_RenderDrawData(ImDrawData* draw_data);

#ifdef IMGUI_IMPL_SGD_IMPLEMENTATION

#include <sgd/sgd.h>

extern ImGuiID ImHashData(const void* data_p, size_t data_size, ImU32 seed = 0);

namespace {

struct ImGuiProcs {

	// globals
	ImGuiID (*ImHashData)(const void*, size_t, ImU32);

	// namespace ImGui members
	ImGuiContext* (*GetCurrentContext)();
	ImGuiViewport* (*GetMainViewport)();
	int (*GetMouseCursor)();
	void* (*MemAlloc)(size_t);
	void (*MemFree)(void*);
	bool (*DebugCheckVersionAndDataLayout)(const char* version_str, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_drawvert, size_t sz_drawidx);
	ImGuiIO& (*GetIO)();
	ImGuiPlatformIO& (*GetPlatformIO)();

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
	void (ImGuiStorage::*SetVoidPtr)(ImGuiID key, void* value);

	// Struct ImFontAtlas members
	void (ImFontAtlas::*GetTexDataAsRGBA32)(unsigned char**, int*, int*, int*);
};

ImGuiProcs procs{&ImHashData, //
				 &ImGui::GetCurrentContext,
				 &ImGui::GetMainViewport,
				 &ImGui::GetMouseCursor,
				 &ImGui::MemAlloc,
				 &ImGui::MemFree,
				 &ImGui::DebugCheckVersionAndDataLayout,
				 &ImGui::GetIO,
				 &ImGui::GetPlatformIO,
				 &ImGuiIO::AddKeyEvent,
				 &ImGuiIO::AddKeyAnalogEvent,
				 &ImGuiIO::AddMousePosEvent,
				 &ImGuiIO::AddMouseButtonEvent,
				 &ImGuiIO::AddMouseWheelEvent,
				 &ImGuiIO::AddMouseSourceEvent,
				 &ImGuiIO::AddFocusEvent,
				 &ImGuiIO::AddInputCharacter,
				 &ImGuiIO::SetKeyEventNativeData,
				 &ImGuiStorage::GetVoidPtr,
				 &ImGuiStorage::SetVoidPtr,
				 &ImFontAtlas::GetTexDataAsRGBA32};
} // namespace

bool ImGui_ImplSGD_Init() {
	return sgd_ImGui_ImplSGD_Init(&procs);
}

void ImGui_ImplSGD_Shutdown() {
	sgd_ImGui_ImplSGD_Shutdown();
}

void ImGui_ImplSGD_NewFrame() {
	sgd_ImGui_ImplSGD_NewFrame();
}

void ImGui_ImplSGD_RenderDrawData(ImDrawData* draw_data) {
	sgd_ImGui_ImplSGD_RenderDrawData(draw_data);
}

#endif	// #ifdef IMGUI_IMPL_SGD_IMPLEMENTATION

#endif // #ifndef IMGUI_DISABLE
