//
// Created by Alex on 12/20/2020.
//

#include "GameWindow.hpp"

using RPG::GameWindow;

struct GameWindow::Internal {

	bool isOpened;

	Internal(bool value) : isOpened(value) {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Game", &isOpened);



		ImGui::End();
	}
};

GameWindow::GameWindow(bool isOpened) : internal(RPG::MakeInternalPointer<Internal>(isOpened)) {}

void GameWindow::Render(ImGuiID dockID) {
	internal->Render(dockID);
}