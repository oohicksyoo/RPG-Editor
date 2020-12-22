//
// Created by Alex on 12/22/2020.
//

#include "HierarchyWindow.hpp"

using RPG::HierarchyWindow;

struct HierarchyWindow::Internal {
	bool isOpened;

	Internal(bool value) : isOpened(value) {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Hierarchy", &isOpened);



		ImGui::End();
	}
};

HierarchyWindow::HierarchyWindow(bool isOpened) : internal(RPG::MakeInternalPointer<Internal>(isOpened)) {}

void HierarchyWindow::Render(ImGuiID dockID) {
	internal->Render(dockID);
}

RPG::Action<>::Callback HierarchyWindow::ToggleIsOpen() {
	return [this] () {
		internal->isOpened = !internal->isOpened;
	};
}

RPG::Action<>::Func<bool> HierarchyWindow::IsOpen() {
	return [this] () -> bool {
		return internal->isOpened;
	};
}
