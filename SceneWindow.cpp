//
// Created by Alex on 12/20/2020.
//

#include "SceneWindow.hpp"

using RPG::SceneWindow;

struct SceneWindow::Internal {

	uint32_t frameBufferID = 1;
	bool isOpened = true;

	Internal() {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Scene", &isOpened);

		//TODO: Figure this out to the same aspect ratio we are currently rendering at
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImGui::Image((void *) (intptr_t) frameBufferID, size, ImVec2{0, 1}, ImVec2{1, 0});

		ImGui::End();
	}

	void SetFrameBuffer(uint32_t id) {
		frameBufferID = id;
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

void SceneWindow::SetFrameBuffer(uint32_t frameBufferID) {
	internal->SetFrameBuffer(frameBufferID);
}