//
// Created by Alex on 12/20/2020.
//

#include "EditorManager.hpp"
#include "ImGUIWrapper.hpp"
#include "IEditorWindow.hpp"
#include "../engine/core/Log.hpp"
#include "../engine/core/SceneManager.hpp"
#include <vector>
#include <memory>

//Editor Windows
#include "MainMenuBarWindow.hpp"
#include "SceneWindow.hpp"
#include "GameWindow.hpp"
#include "HierarchyWindow.hpp"
#include "InspectorWindow.hpp"
#include "AssetWindow.hpp"
#include "InputHelperWindow.hpp"
#include "ConsoleWindow.hpp"
#include "EditorStats.hpp"
#include "MaterialMakerWindow.hpp"

using RPG::EditorManager;

struct EditorManager::Internal {
	bool isGameRunning;
	std::vector<std::shared_ptr<RPG::IEditorWindow>> editorWindows;
	std::shared_ptr<RPG::MainMenuBarWindow> mainMenuBarWindow;
	std::shared_ptr<RPG::SceneWindow> sceneWindow;
	std::shared_ptr<RPG::GameWindow> gameWindow;
	std::shared_ptr<RPG::HierarchyWindow> hierarchyWindow;
	std::shared_ptr<RPG::InspectorWindow> inspectorWindow;
	std::shared_ptr<RPG::AssetWindow> assetWindow;
	std::shared_ptr<RPG::InputHelperWindow> inputHelperWindow;
	std::shared_ptr<RPG::ConsoleWindow> consoleWindow;
    std::shared_ptr<RPG::MaterialMakerWindow> materialMakerWindow;

	Internal(const RPG::SDLWindow& window, SDL_GLContext context) : isGameRunning(false) {
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
		SetupStyling();

		//Setup Platform/IRenderer Bindings
		ImGui_ImplSDL2_InitForOpenGL(window.GetWindow(), context);
		ImGui_ImplOpenGL3_Init(glslVersion);

		//Setup Base windows
		sceneWindow = std::make_unique<RPG::SceneWindow>();
		gameWindow = std::make_unique<RPG::GameWindow>();

		//Setup Inspector Window
		inspectorWindow = std::make_unique<RPG::InspectorWindow>();

		//Setup Hierarchy Window
		hierarchyWindow = std::make_unique<RPG::HierarchyWindow>();
		hierarchyWindow->SelectedGameObjectAction([this](std::shared_ptr<RPG::GameObject> gameObject) {
			RPG::EditorStats::GetInstance().SetSelectedGameObject(gameObject);
			inspectorWindow->SetSelectedGameObject(gameObject);
			sceneWindow->SetSelectedGameObject(gameObject);
		});

		//Setup AssetWindow
		assetWindow = std::make_unique<RPG::AssetWindow>();

		//Setup Input Helper Window
		inputHelperWindow = std::make_unique<RPG::InputHelperWindow>();

		//Console
		consoleWindow = std::make_unique<RPG::ConsoleWindow>();

		//Material Maker
		materialMakerWindow = std::make_unique<RPG::MaterialMakerWindow>();

		mainMenuBarWindow = std::make_unique<RPG::MainMenuBarWindow>();
		mainMenuBarWindow->AddToggleableEditorWindow({"Scene", sceneWindow->ToggleIsOpen(), sceneWindow->IsOpen()});
		mainMenuBarWindow->AddToggleableEditorWindow({"Game", gameWindow->ToggleIsOpen(), gameWindow->IsOpen()});
		mainMenuBarWindow->AddToggleableEditorWindow({"Hierarchy", hierarchyWindow->ToggleIsOpen(), hierarchyWindow->IsOpen()});
		mainMenuBarWindow->AddToggleableEditorWindow({"Inspector", inspectorWindow->ToggleIsOpen(), inspectorWindow->IsOpen()});
		mainMenuBarWindow->AddToggleableEditorWindow({"Assets", assetWindow->ToggleIsOpen(), assetWindow->IsOpen()});
		mainMenuBarWindow->AddToggleableEditorWindow({"Input Helper", inputHelperWindow->ToggleIsOpen(), inputHelperWindow->IsOpen()});
		//mainMenuBarWindow->AddToggleableEditorWindow({"Console", consoleWindow->ToggleIsOpen(), consoleWindow->IsOpen()});
        mainMenuBarWindow->AddToggleableEditorWindow({"Material Maker", materialMakerWindow->ToggleIsOpen(), materialMakerWindow->IsOpen()});
		mainMenuBarWindow->PlayToggleAction([this]() {
			isGameRunning = !isGameRunning;

			if (isGameRunning) {
				//We need to store the current hierarchy
				RPG::SceneManager::GetInstance().StoreCurrentScene();
			} else {
				//We need to restore the stored hierarchy to reset everything
				RPG::SceneManager::GetInstance().ReloadCurrentScene();
			}
		});
		mainMenuBarWindow->PlayToggleFunc([this]() -> bool {
			return isGameRunning;
		});

		editorWindows.push_back(std::shared_ptr<RPG::IEditorWindow>(mainMenuBarWindow));
		editorWindows.push_back(std::shared_ptr<RPG::IEditorWindow>(sceneWindow));
		editorWindows.push_back(std::shared_ptr<RPG::IEditorWindow>(gameWindow));
		editorWindows.push_back(std::shared_ptr<RPG::IEditorWindow>(hierarchyWindow));
		editorWindows.push_back(std::shared_ptr<RPG::IEditorWindow>(inspectorWindow));
		editorWindows.push_back(std::shared_ptr<RPG::IEditorWindow>(assetWindow));
		editorWindows.push_back(std::shared_ptr<RPG::IEditorWindow>(inputHelperWindow));
		//editorWindows.push_back(std::shared_ptr<RPG::IEditorWindow>(consoleWindow));
        editorWindows.push_back(std::shared_ptr<RPG::IEditorWindow>(materialMakerWindow));

		RPG::Log("EditorManager", "Initialization complete");
	}

	void SetupStyling() {
		ImGuiStyle& style = ImGui::GetStyle();

		style.Colors[ImGuiCol_Text]                   = ImVec4{1.000f, 1.000f, 1.000f, 1.000f};
		style.Colors[ImGuiCol_TextDisabled]           = ImVec4{0.500f, 0.500f, 0.500f, 1.000f};
		style.Colors[ImGuiCol_WindowBg]               = ImVec4{0.180f, 0.180f, 0.180f, 1.000f};
		style.Colors[ImGuiCol_ChildBg]                = ImVec4{0.280f, 0.280f, 0.280f, 0.000f};
		style.Colors[ImGuiCol_PopupBg]                = ImVec4{0.313f, 0.313f, 0.313f, 1.000f};
		style.Colors[ImGuiCol_Border]                 = ImVec4{0.266f, 0.266f, 0.266f, 1.000f};
		style.Colors[ImGuiCol_BorderShadow]           = ImVec4{0.000f, 0.000f, 0.000f, 0.000f};
		style.Colors[ImGuiCol_FrameBg]                = ImVec4{0.160f, 0.160f, 0.160f, 1.000f};
		style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4{0.200f, 0.200f, 0.200f, 1.000f};
		style.Colors[ImGuiCol_FrameBgActive]          = ImVec4{0.280f, 0.280f, 0.280f, 1.000f};
		style.Colors[ImGuiCol_TitleBg]                = ImVec4{0.148f, 0.148f, 0.148f, 1.000f};
		style.Colors[ImGuiCol_TitleBgActive]          = ImVec4{0.148f, 0.148f, 0.148f, 1.000f};
		style.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4{0.148f, 0.148f, 0.148f, 1.000f};
		style.Colors[ImGuiCol_MenuBarBg]              = ImVec4{0.195f, 0.195f, 0.195f, 1.000f};
		style.Colors[ImGuiCol_ScrollbarBg]            = ImVec4{0.160f, 0.160f, 0.160f, 1.000f};
		style.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4{0.277f, 0.277f, 0.277f, 1.000f};
		style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4{0.300f, 0.300f, 0.300f, 1.000f};
		style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4{1.000f, 0.391f, 0.000f, 1.000f};
		style.Colors[ImGuiCol_CheckMark]              = ImVec4{1.000f, 1.000f, 1.000f, 1.000f};
		style.Colors[ImGuiCol_SliderGrab]             = ImVec4{0.391f, 0.391f, 0.391f, 1.000f};
		style.Colors[ImGuiCol_SliderGrabActive]       = ImVec4{1.000f, 0.391f, 0.000f, 1.000f};
		style.Colors[ImGuiCol_Button]                 = ImVec4{1.000f, 1.000f, 1.000f, 0.000f};
		style.Colors[ImGuiCol_ButtonHovered]          = ImVec4{1.000f, 1.000f, 1.000f, 0.156f};
		style.Colors[ImGuiCol_ButtonActive]           = ImVec4{1.000f, 1.000f, 1.000f, 0.391f};
		style.Colors[ImGuiCol_Header]                 = ImVec4{0.313f, 0.313f, 0.313f, 1.000f};
		style.Colors[ImGuiCol_HeaderHovered]          = ImVec4{0.469f, 0.469f, 0.469f, 1.000f};
		style.Colors[ImGuiCol_HeaderActive]           = ImVec4{0.469f, 0.469f, 0.469f, 1.000f};
		style.Colors[ImGuiCol_Separator]              = ImVec4{0.266f, 0.266f, 0.266f, 1.000f};
		style.Colors[ImGuiCol_SeparatorHovered]       = ImVec4{0.391f, 0.391f, 0.391f, 1.000f};
		style.Colors[ImGuiCol_SeparatorActive]        = ImVec4{1.000f, 0.391f, 0.000f, 1.000f};
		style.Colors[ImGuiCol_ResizeGrip]             = ImVec4{1.000f, 1.000f, 1.000f, 0.250f};
		style.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4{1.000f, 1.000f, 1.000f, 0.670f};
		style.Colors[ImGuiCol_ResizeGripActive]       = ImVec4{1.000f, 0.391f, 0.000f, 1.000f};
		style.Colors[ImGuiCol_Tab]                    = ImVec4{0.098f, 0.098f, 0.098f, 1.000f};
		style.Colors[ImGuiCol_TabHovered]             = ImVec4{0.352f, 0.352f, 0.352f, 1.000f};
		style.Colors[ImGuiCol_TabActive]              = ImVec4{0.195f, 0.195f, 0.195f, 1.000f};
		style.Colors[ImGuiCol_TabUnfocused]           = ImVec4{0.098f, 0.098f, 0.098f, 1.000f};
		style.Colors[ImGuiCol_TabUnfocusedActive]     = ImVec4{0.195f, 0.195f, 0.195f, 1.000f};
		style.Colors[ImGuiCol_DockingPreview]         = ImVec4{1.000f, 0.391f, 0.000f, 0.781f};
		style.Colors[ImGuiCol_DockingEmptyBg]         = ImVec4{0.180f, 0.180f, 0.180f, 1.000f};
		style.Colors[ImGuiCol_PlotLines]              = ImVec4{0.469f, 0.469f, 0.469f, 1.000f};
		style.Colors[ImGuiCol_PlotLinesHovered]       = ImVec4{1.000f, 0.391f, 0.000f, 1.000f};
		style.Colors[ImGuiCol_PlotHistogram]          = ImVec4{0.586f, 0.586f, 0.586f, 1.000f};
		style.Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4{1.000f, 0.391f, 0.000f, 1.000f};
		style.Colors[ImGuiCol_TextSelectedBg]         = ImVec4{1.000f, 1.000f, 1.000f, 0.156f};
		style.Colors[ImGuiCol_DragDropTarget]         = ImVec4{1.000f, 0.391f, 0.000f, 1.000f};
		style.Colors[ImGuiCol_NavHighlight]           = ImVec4{1.000f, 0.391f, 0.000f, 1.000f};
		style.Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4{1.000f, 0.391f, 0.000f, 1.000f};
		style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4{0.000f, 0.000f, 0.000f, 0.586f};
		style.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4{0.000f, 0.000f, 0.000f, 0.586f};

		style.ChildRounding = 4.0f;
		style.FrameBorderSize = 1.0f;
		style.FrameRounding = 2.0f;
		style.GrabMinSize = 7.0f;
		style.PopupRounding = 2.0f;
		style.ScrollbarRounding = 12.0f;
		style.ScrollbarSize = 13.0f;
		style.TabBorderSize = 1.0f;
		style.TabRounding = 0.0f;
		style.WindowRounding = 4.0f;
	}

	void NewFrame(const RPG::SDLWindow& window) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window.GetWindow());
		ImGui::NewFrame();
	}

	void BuildGUI(std::shared_ptr<FrameBuffer> frameBuffer, std::shared_ptr<RPG::FrameBuffer> gameFrameBuffer, std::shared_ptr<RPG::Hierarchy> hierarchy) {
		ImGuiID dockSpaceID = ImGui::DockSpaceOverViewport();

		sceneWindow->SetFrameBuffer(frameBuffer->GetRenderTextureID());
		gameWindow->SetFrameBuffer(gameFrameBuffer->GetRenderTextureID());
		hierarchyWindow->SetHierarchy(hierarchy);

		for (auto&& editorWindow : editorWindows) {
			editorWindow->Render(dockSpaceID);
		}

		//ImGui::ShowDemoWindow();
	}

	void Render() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void OnGeneralEventData(SDL_Event event) {
		ImGui_ImplSDL2_ProcessEvent(&event);
	}

	void SubmitMaterialMakerFrameBuffer(std::shared_ptr<RPG::FrameBuffer> frameBuffer) {
		materialMakerWindow->SubmitFramebuffer(frameBuffer);
	}
};

EditorManager::EditorManager(const RPG::SDLWindow& window, SDL_GLContext context) : internal(RPG::MakeInternalPointer<Internal>(window, context)) {}

void EditorManager::NewFrame(const RPG::SDLWindow& window) {
	internal->NewFrame(window);
}

void EditorManager::BuildGUI(std::shared_ptr<RPG::FrameBuffer> frameBuffer, std::shared_ptr<RPG::FrameBuffer> gameFrameBuffer, std::shared_ptr<RPG::Hierarchy> hierarchy) {
	internal->BuildGUI(frameBuffer, gameFrameBuffer, hierarchy);
}

void EditorManager::Render() {
	internal->Render();
}

bool EditorManager::IsGameRunning() {
	return internal->isGameRunning;
}

void EditorManager::OnGeneralEventData(SDL_Event event) {
	internal->OnGeneralEventData(event);
}

void RPG::EditorManager::SubmitMaterialMakerFrameBuffer(std::shared_ptr<RPG::FrameBuffer> frameBuffer) {
	internal->SubmitMaterialMakerFrameBuffer(frameBuffer);
}

std::shared_ptr<RPG::Material> RPG::EditorManager::GetCurrentMaterial() {
    return internal->materialMakerWindow->GetCurrentMaterial();
}
