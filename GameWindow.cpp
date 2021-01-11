//
// Created by Alex on 12/20/2020.
//

#include "GameWindow.hpp"
#include "../engine/application/ApplicationStats.hpp"

using RPG::GameWindow;

struct GameWindow::Internal {
	uint32_t frameBufferID;
	bool isOpened = true;

	Internal() {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Game", &isOpened);

		ImVec2 size = ImGui::GetContentRegionAvail();
		ImGui::Image((void *) (intptr_t) frameBufferID, CalculateContentSize(size), ImVec2{0, 1}, ImVec2{1, 0});

		ImGui::End();
		ImGui::PopStyleVar();
	}

	ImVec2 CalculateContentSize(ImVec2 availableSize) {
		auto size = RPG::ApplicationStats::GetInstance().GetRenderingSize();
		ImVec2 frameBufferSize{size.x, size.y};

		float width = frameBufferSize.x;
		float height = frameBufferSize.y;
		float ratio = frameBufferSize.x / frameBufferSize.y;
		float reciprocal = 1 / ratio;

		height = availableSize.x * reciprocal;

		if (height > availableSize.y) {
			height = availableSize.y;
		}

		width = height * ratio;

		return ImVec2{width, height};
	}

	void SetFrameBuffer(uint32_t id) {
		frameBufferID = id;
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

void GameWindow::SetFrameBuffer(uint32_t frameBufferID) {
	internal->SetFrameBuffer(frameBufferID);
}