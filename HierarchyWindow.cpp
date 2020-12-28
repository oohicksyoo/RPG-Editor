//
// Created by Alex on 12/22/2020.
//

#include "HierarchyWindow.hpp"
#include "../engine/core/GameObject.hpp"

using RPG::HierarchyWindow;

struct HierarchyWindow::Internal {
	bool isOpened;
	std::shared_ptr<RPG::Hierarchy> hierarchy;

	Internal() : isOpened(true) {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Hierarchy", &isOpened);

		for (std::shared_ptr<RPG::GameObject> gameObject : hierarchy->GetHierarchy()) {
			RenderGameObject(gameObject);
		}

		ImGui::End();
	}

	void RenderGameObject(std::shared_ptr<RPG::GameObject> gameObject) {
		bool isSelected = false; //TODO: Change this to check gameObject guid to current selected guid
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
		if (!gameObject->HasChildren()) {
			flags |= ImGuiTreeNodeFlags_Leaf;
		} else {
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		}

		//Selected goes here

		bool isOpen = ImGui::TreeNodeEx((void*)(intptr_t)0, flags, gameObject->GetName().c_str());

		//Check click

		if (isOpen) {
			for (std::shared_ptr<RPG::GameObject> childGameObject : gameObject->GetChildren()) {
				RenderGameObject(childGameObject);
			}
			ImGui::TreePop();
		}
	}
};

HierarchyWindow::HierarchyWindow() : internal(RPG::MakeInternalPointer<Internal>()) {}

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

void HierarchyWindow::SetHierarchy(std::shared_ptr<RPG::Hierarchy> hierarchy) {
	internal->hierarchy = hierarchy;
}
