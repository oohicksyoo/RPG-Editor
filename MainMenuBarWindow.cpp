//
// Created by Alex on 12/20/2020.
//

#include "MainMenuBarWindow.hpp"
#include "../engine/core/Log.hpp"
#include "../engine/core/Action.hpp"

using RPG::MainMenuBarWindow;

struct MainMenuBarWindow::Internal {

	RPG::Action<> sceneOnToggleAction = {};
	RPG::Action<>::Callback firstFunc = [this]() {FirstFunc();};

	Internal() {
		sceneOnToggleAction += []() {
			RPG::Log("Main Menu Bar Window", "Scene Toggle");
		};
		sceneOnToggleAction += firstFunc;
		sceneOnToggleAction += [this]() {SecondFunc();};
	}

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
				if (ImGui::MenuItem("Scene",0, true)) {
					if (sceneOnToggleAction.GetListenerCount() > 0) {
						sceneOnToggleAction.Invoke();
						sceneOnToggleAction -= firstFunc;
					}
				}
				if (ImGui::MenuItem("Game", 0, true)) {}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void FirstFunc() {
		RPG::Log("Main Menu Bar Window", "First Func");
	}

	void SecondFunc() {
		RPG::Log("Main Menu Bar Window", "Second Func");
	}

	void ThirdFunc() {
		RPG::Log("Main Menu Bar Window", "Third Func");
	}
};

MainMenuBarWindow::MainMenuBarWindow() : internal(RPG::MakeInternalPointer<Internal>()) {}

void MainMenuBarWindow::Render(ImGuiID dockID) {
	internal->Render(dockID);
}