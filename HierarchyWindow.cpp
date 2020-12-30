//
// Created by Alex on 12/22/2020.
//

#include "HierarchyWindow.hpp"
#include "../engine/core/Log.hpp"

using RPG::HierarchyWindow;

struct HierarchyWindow::Internal {
	bool isOpened;
	std::shared_ptr<RPG::Hierarchy> hierarchy;
	std::string selectedGuid;
	std::shared_ptr<RPG::GameObject> selectedGameObject;

	Internal() : isOpened(true),
				 selectedGuid(""),
				 selectedGameObject(nullptr) {}

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
		bool isSelected = selectedGuid == gameObject->GetGuid();
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
		if (!gameObject->HasChildren()) {
			flags |= ImGuiTreeNodeFlags_Leaf;
		} else {
			flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		}

		//Selected goes here
		if (isSelected) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool isOpen = ImGui::TreeNodeEx(gameObject->GetGuid().c_str(), flags, gameObject->GetName().c_str());

		//Check click
		if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing()) {
			selectedGuid = isSelected ? "" : gameObject->GetGuid();
			selectedGameObject = isSelected ? nullptr : gameObject;
		}

		//Drag and Drop
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("GameObject", &gameObject, sizeof(RPG::GameObject));
			std::string str = "GameObject: " + gameObject->GetName();
			ImGui::Text(str.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject")) {
				IM_ASSERT(payload->DataSize == sizeof(RPG::GameObject));
				RPG::Log("Hierarchy Window" , "Payload for a GameObject arrived");
				//RPG::GameObject payload_n = std::static_pointer_cast<RPG::GameObject>(payload->Data);
			}
			ImGui::EndDragDropTarget();
		}

		//Render out children
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
