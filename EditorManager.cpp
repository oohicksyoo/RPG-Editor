//
// Created by Alex on 12/20/2020.
//

#include "EditorManager.hpp"
#include "ImGUIWrapper.hpp"
#include "../engine/core/Log.hpp"

using RPG::EditorManager;

struct EditorManager::Internal {
	Internal(const RPG::SDLWindow& window, SDL_GLContext context) {
		RPG::Log("EditorManager", "Starting up the editor");
		Initialize(window, context);
	}

	~Internal() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void Initialize(const RPG::SDLWindow& window, SDL_GLContext context) {
		RPG::Log("EditorManager", "Initialization");

		#ifdef USING_GLES
			const char* glslVersion = "#version 100";
		#else
			const char* glslVersion = "#version 120";
		#endif

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		//Style
		ImGui::StyleColorsDark();

		//Setup Platform/Renderer Bindings
		ImGui_ImplSDL2_InitForOpenGL(window.GetWindow(), context);
		ImGui_ImplOpenGL3_Init(glslVersion);

		RPG::Log("EditorManager", "Initialization complete");
	}

	void NewFrame(const RPG::SDLWindow& window) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window.GetWindow());
		ImGui::NewFrame();
	}

	void BuildGUI() {
		ImGui::ShowDemoWindow();
	}

	void Render() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};

EditorManager::EditorManager(const RPG::SDLWindow& window, SDL_GLContext context) : internal(RPG::MakeInternalPointer<Internal>(window, context)) {}

void EditorManager::NewFrame(const RPG::SDLWindow& window) {
	internal->NewFrame(window);
}

void EditorManager::BuildGUI() {
	internal->BuildGUI();
}

void EditorManager::Render() {
	internal->Render();
}
