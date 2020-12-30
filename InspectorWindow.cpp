//
// Created by Alex on 12/30/2020.
//

#include "InspectorWindow.hpp"
#include "../engine/core/Log.hpp"

using RPG::InspectorWindow;

struct InspectorWindow::Internal {
	bool isOpened;

	Internal() : isOpened(true) {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Inspector", &isOpened);


		ImGui::End();
	}
};

InspectorWindow::InspectorWindow() : internal(RPG::MakeInternalPointer<Internal>()) {}

void InspectorWindow::Render(ImGuiID dockID) {
	internal->Render(dockID);
}

RPG::Action<>::Callback InspectorWindow::ToggleIsOpen() {
	return [this] () {
		internal->isOpened = !internal->isOpened;
	};
}

RPG::Action<>::Func<bool> InspectorWindow::IsOpen() {
	return [this] () -> bool {
		return internal->isOpened;
	};
}