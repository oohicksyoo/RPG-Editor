//
// Created by Alex on 12/20/2020.
//

#include "MainMenuBarWindow.hpp"

using RPG::MainMenuBarWindow;

struct MainMenuBarWindow::Internal {

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
				if (ImGui::MenuItem("Scene")) {}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
};

MainMenuBarWindow::MainMenuBarWindow() : internal(RPG::MakeInternalPointer<Internal>()) {}

void MainMenuBarWindow::Render(ImGuiID dockID) {
	internal->Render(dockID);
}