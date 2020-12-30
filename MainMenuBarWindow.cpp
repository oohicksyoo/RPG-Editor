//
// Created by Alex on 12/20/2020.
//

#include "MainMenuBarWindow.hpp"
#include "../engine/core/Log.hpp"
#include <vector>

using RPG::MainMenuBarWindow;

struct MainMenuBarWindow::Internal {

	std::vector<RPG::MainMenuBarEditorWindowToggle> editorWindowToggles;
	RPG::Action<> playToggleAction = {};
	RPG::Action<>::Func<bool> playToggleFunc = {};

	Internal() {}

	void Render(ImGuiID dockID) {
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Save", "CTRL+S")) {}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Windows")) {
				for(auto editorWindowToggle : editorWindowToggles) {
					if (ImGui::MenuItem(editorWindowToggle.windowName.c_str(), 0, editorWindowToggle.onIsOpenFunc())) {
						editorWindowToggle.onToggleAction();
					}
				}
				ImGui::EndMenu();
			}

			std::string playButtonText = playToggleFunc() ? "Stop" : "Start";
			if (ImGui::MenuItem(playButtonText.c_str())) {
				playToggleAction.Invoke();
			}

			ImGui::EndMainMenuBar();
		}
	}

	bool IsOpened() {
		return true;
	}

	void SetOpenState(bool value) {}
};

MainMenuBarWindow::MainMenuBarWindow() : internal(RPG::MakeInternalPointer<Internal>()) {}

void MainMenuBarWindow::Render(ImGuiID dockID) {
	internal->Render(dockID);
}

RPG::Action<>::Callback MainMenuBarWindow::ToggleIsOpen() {
	return [this] () {};
}

RPG::Action<>::Func<bool> MainMenuBarWindow::IsOpen() {
	return [this] () -> bool {
		return true;
	};
}

void MainMenuBarWindow::PlayToggleAction(RPG::Action<>::Callback callback) {
	internal->playToggleAction += callback;
}

void MainMenuBarWindow::PlayToggleFunc(RPG::Action<>::Func<bool> callback) {
	internal->playToggleFunc = callback;
}

void MainMenuBarWindow::AddToggleableEditorWindow(RPG::MainMenuBarEditorWindowToggle editorWindow) {
	internal->editorWindowToggles.push_back(editorWindow);
}