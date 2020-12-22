//
// Created by Alex on 12/20/2020.
//

#include "SceneWindow.hpp"

using RPG::SceneWindow;

struct SceneWindow::Internal {

	bool isOpened;

	Internal() : isOpened(true) {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Scene", &isOpened);



		ImGui::End();
	}
};

SceneWindow::SceneWindow() : internal(RPG::MakeInternalPointer<Internal>()) {}

void SceneWindow::Render(ImGuiID dockID) {
	internal->Render(dockID);
}

RPG::Action<>::Callback SceneWindow::ToggleIsOpen() {
	return [this] () {
		internal->isOpened = !internal->isOpened;
	};
}

RPG::Action<>::Func<bool> SceneWindow::IsOpen() {
	return [this] () -> bool {
		return internal->isOpened;
	};
}