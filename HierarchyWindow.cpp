//
// Created by Alex on 12/22/2020.
//

#include "HierarchyWindow.hpp"
#include "../engine/core/Log.hpp"
#include "payloads/GameObjectPayload.hpp"
#include "EditorStats.hpp"
#include "../engine/core/Serializer.hpp"

using RPG::HierarchyWindow;

struct HierarchyWindow::Internal {
	bool isOpened;
	std::shared_ptr<RPG::Hierarchy> hierarchy;
	std::string selectedGuid;
	std::shared_ptr<RPG::GameObject> selectedGameObject;
	RPG::Action<std::shared_ptr<RPG::GameObject>> selectedGameObjectAction = {};

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

			auto selected = RPG::EditorStats::GetInstance().GetSelectedGameObject();
			if (selected != nullptr) {
				std::string s = "Duplicate \"" + selected->GetName() + "\"";
				if (ImGui::Button(s.c_str())) {
					auto j = RPG::Serializer::GetInstance().SaveGameObject(selectedGameObject);
					auto go = RPG::Serializer::GetInstance().LoadGameObject(j, true);
					auto parent = selectedGameObject->GetParent();
					if (parent != nullptr) {
						go->SetParent(go, parent);
					} else {
						hierarchy->Add(go);
					}

					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}

		ImGui::Separator();
		//Drop - payload
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject")) {
				IM_ASSERT(payload->DataSize == sizeof(RPG::GameObjectPayload));
				RPG::Log("Hierarchy Window" , "Payload for a GameObject arrived");
				RPG::GameObjectPayload p = *(const RPG::GameObjectPayload*)payload->Data;
				if (!p.gameObject->HasParent()) {
					hierarchy->Remove(p.gameObject);
				}
				p.gameObject->SetParent(p.gameObject, nullptr);
				hierarchy->Add(p.gameObject);
			}
			ImGui::EndDragDropTarget();
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

			//Fire off our selected GameObject
			if (selectedGameObjectAction.GetListenerCount() > 0) {
				selectedGameObjectAction.Invoke(selectedGameObject);
			}
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
				if (gameObject->GetGuid() != p.guid) {
					if (!p.gameObject->HasParent()) {
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

void HierarchyWindow::SetSelectedGameObject(std::shared_ptr<RPG::GameObject> gameObject) {
    internal->selectedGameObject = gameObject;
    internal->selectedGuid = gameObject == nullptr ? "" : gameObject->GetGuid();;
}

void HierarchyWindow::SelectedGameObjectAction(RPG::Action<std::shared_ptr<RPG::GameObject>>::Callback callback) {
	internal->selectedGameObjectAction += callback;
}
