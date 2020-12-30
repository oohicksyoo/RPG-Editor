//
// Created by Alex on 12/22/2020.
//

#include "HierarchyWindow.hpp"
#include "../engine/core/Log.hpp"
#include "payloads/GameObjectPayload.hpp"

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

		if (ImGui::BeginPopupContextWindow("Context Menu")) {
			if (ImGui::Button("New GameObject")) {
				hierarchy->Add(std::make_unique<RPG::GameObject>(RPG::GameObject()));
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
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

		//Drag - payload
		if (ImGui::BeginDragDropSource()) {
			RPG::GameObjectPayload payload = RPG::GameObjectPayload();
			payload.guid = gameObject->GetGuid();
			payload.gameObject = gameObject;

			ImGui::SetDragDropPayload("GameObject", &payload, sizeof(RPG::GameObjectPayload));
			std::string str = "GameObject: " + gameObject->GetName();
			ImGui::Text(str.c_str());
			ImGui::EndDragDropSource();
		}

		//Drop - payload
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject")) {
				IM_ASSERT(payload->DataSize == sizeof(RPG::GameObjectPayload));
				RPG::Log("Hierarchy Window" , "Payload for a GameObject arrived");
				RPG::GameObjectPayload p = *(const RPG::GameObjectPayload*)payload->Data;
				RPG::Log("Hierarchy Window", "Guid: " + p.guid);
				RPG::Log("Hierarchy Window", "Guid: " + p.gameObject->GetGuid());
				if (gameObject->GetGuid() != p.guid) {
					if (!p.gameObject->HasParent()) {
						RPG::Log("Hierarchy Window", "Removing gameobject from start of hierarchy");
						hierarchy->Remove(p.gameObject);
					}
					p.gameObject->SetParent(p.gameObject, gameObject);
				}
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
