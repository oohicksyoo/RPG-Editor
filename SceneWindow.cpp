//
// Created by Alex on 12/20/2020.
//

#include "SceneWindow.hpp"

using RPG::SceneWindow;

struct SceneWindow::Internal {

	bool isOpened;

	Internal(bool value) : isOpened(value) {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Scene", &isOpened);



		ImGui::End();
	}
};

SceneWindow::SceneWindow(bool isOpened) : internal(RPG::MakeInternalPointer<Internal>(isOpened)) {}

void SceneWindow::Render(ImGuiID dockID) {
	internal->Render(dockID);
}