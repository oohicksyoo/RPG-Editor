//
// Created by Alex on 12/30/2020.
//

#include "InspectorWindow.hpp"
#include "../engine/core/Log.hpp"
#include <regex>

using RPG::InspectorWindow;

struct InspectorWindow::Internal {
	bool isOpened;
	std::shared_ptr<RPG::GameObject> selectedGameObject;

	Internal() : isOpened(true),
				 selectedGameObject(nullptr) {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Inspector", &isOpened);
		if (selectedGameObject == nullptr) { ImGui::End(); return; }

		std::string name = selectedGameObject->GetName();
		if (ImGui::InputText("Name", &name)) {
			selectedGameObject->SetName(name);
		}

		ImGui::TextDisabled(selectedGameObject->GetGuid().c_str());
		ImGui::SameLine();
		ImGui::Text("Guid");

		if (ImGui::Button("Add Component")) {
			ImGui::OpenPopup("Add Component");
		}
		ImGui::SameLine();
		bool isDeleted = ImGui::Button("Delete GameObject");

		//Render Current Components
		{
			for (auto component : selectedGameObject->GetComponents()) {
				ImGui::Separator();
				ImGui::BeginGroup();
				if (ImGui::CollapsingHeader(SplitName(component->Name()).c_str())) {
					//Render Component Internals
				}
				ImGui::EndGroup();
			}
		}

		//Add Component
		{
			//TODO: This will need to be dynamic some how in the future for project to add c++ level components if they cant rely on lua everytime
			if (ImGui::BeginPopup("Add Component")) {
				ImGui::Text("Components");
				ImGui::Separator();
				if (ImGui::Selectable("Camera Component")) {
					//TODO: Add Camera Component
				}
				ImGui::EndPopup();
			}
		}

		//Deleting GameObject and preforming cleanup
		if (isDeleted) {
			selectedGameObject->SetParent(selectedGameObject, nullptr); //Clear my parent relation ship; deleting children as well
			//TODO: Currently if anyone has a reference to the pointer it doesnt get shut down right away
			//Deleting a child for example with release at the end of the application and not right away
			//If all children are removed then the main parent will be cleaned  up at the end of application
		}

		ImGui::End();
	}

	std::string SplitName(std::string name) {
		std::regex regex("(?=[^a-z])");
		return std::regex_replace(name, regex, " ");
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

void InspectorWindow::SetSelectedGameObject(std::shared_ptr<RPG::GameObject> gameObject) {
	internal->selectedGameObject = gameObject;
}