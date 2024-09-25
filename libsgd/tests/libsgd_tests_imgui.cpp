// Dear ImGui: standalone example application for Emscripten, using GLFW + WebGPU
// (Emscripten is a C++-to-javascript compiler, used to publish executables for the web. See https://emscripten.org/)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"

#include <sgd/sgd.h>

#define IMGUI_IMPL_SGD_IMPLEMENTATION 1
#include <sgd/imgui_impl_sgd.h>

// Main code
int main(int, char**) {

	sgd_Init();

	sgd_CreateWindow(1280, 720, "Hello World!", SGD_WINDOW_FLAGS_RESIZABLE);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	ImGui_ImplSGD_Init();

	sgd_LoadSkybox("sgd://envmaps/stormy-cube.jpg", .3);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;

	while (!(sgd_PollEvents() & SGD_EVENT_MASK_CLOSE_CLICKED)) {

		// Render SGD Scene
		sgd_RenderScene();

		// Start the Dear ImGui frame
		ImGui_ImplSGD_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn
		// more about Dear ImGui!).
		if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");		   // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f

			if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
						ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window) {
			ImGui::Begin("Another Window", &show_another_window); // Pass a pointer to our bool variable (the window will have a
																  // closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me")) show_another_window = false;
			ImGui::End();
		}

		ImGui::Render();

		ImGui_ImplSGD_RenderDrawData(ImGui::GetDrawData());

		sgd_Present();
	}

	// Cleanup
	ImGui_ImplSGD_Shutdown();
	ImGui::DestroyContext();

	sgd_Terminate();
	return 0;
}
