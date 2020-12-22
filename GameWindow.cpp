//
// Created by Alex on 12/20/2020.
//

#include "GameWindow.hpp"

using RPG::GameWindow;

struct GameWindow::Internal {

	bool isOpened;

	Internal() : isOpened(true) {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Game", &isOpened);



		ImGui::End();
	}
};

GameWindow::GameWindow() : internal(RPG::MakeInternalPointer<Internal>()) {}

void GameWindow::Render(ImGuiID dockID) {
	internal->Render(dockID);
}

RPG::Action<>::Callback GameWindow::ToggleIsOpen() {
	return [this] () {
		internal->isOpened = !internal->isOpened;
	};
}

RPG::Action<>::Func<bool> GameWindow::IsOpen() {
	return [this] () -> bool {
		return internal->isOpened;
	};
}