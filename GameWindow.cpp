//
// Created by Alex on 12/20/2020.
//

#include "GameWindow.hpp"

using RPG::GameWindow;

struct GameWindow::Internal {
	uint32_t frameBufferID = 1;
	bool isOpened = true;

	Internal() {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Game", &isOpened);

		//TODO: Figure this out to the same aspect ratio we are currently rendering at
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImGui::Image((void *) (intptr_t) frameBufferID, size, ImVec2{0, 1}, ImVec2{1, 0});

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