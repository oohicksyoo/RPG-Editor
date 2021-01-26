//
// Created by Alex on 12/30/2020.
//

#include "InspectorWindow.hpp"
#include "../engine/application/ApplicationStats.hpp"
#include "../engine/core/Log.hpp"
#include "../engine/core/Property.hpp"
#include "../engine/core/GLMWrapper.hpp"
#include "../engine/core/components/CameraComponent.hpp"
#include "../engine/core/components/LuaScriptComponent.hpp"
#include "../engine/core/components/MeshComponent.hpp"
#include "../engine/core/components/SpriteComponent.hpp"
#include "../engine/core/components/BoxColliderComponent.hpp"
#include "../engine/core/Action.hpp"
#include "../engine/core/AssetInventory.hpp"
#include "../engine/core/Serializer.hpp"
#include <regex>

using RPG::InspectorWindow;

struct InspectorWindow::Internal {
	bool isOpened;
	std::shared_ptr<RPG::GameObject> selectedGameObject;

	Internal() : isOpened(true),
				 selectedGameObject(nullptr) {
		BuildDefaultPropertyLayouts();
	}

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
				std::string name = SplitName(component->Name()) + "##" + component->Guid();
				if (ImGui::CollapsingHeader(name.c_str())) {
					//Render Component Internals
					for (auto property : component->GetProperties()) {
						RenderProperty(property);
					}
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
					auto size = RPG::ApplicationStats::GetInstance().GetWindowSize();
					std::shared_ptr<RPG::IComponent> component = selectedGameObject->AddComponent(std::make_unique<RPG::CameraComponent>(RPG::CameraComponent(size.x, size.y, selectedGameObject->GetTransform())));
					if (component == nullptr) {
						RPG::Log("SceneMain", "Failed to add (CameraComponent) component to GameObject");
					} else {
						RPG::Log("SceneMain", "Added (" + component->Name() + ") component to GameObject");
					}
				}
				if (ImGui::Selectable("Lua Script Component")) {
					std::shared_ptr<RPG::IComponent> component = selectedGameObject->AddComponent(std::make_unique<RPG::LuaScriptComponent>(RPG::LuaScriptComponent("assets/scripts/Sample.lua", selectedGameObject)));
					if (component == nullptr) {
						RPG::Log("SceneMain", "Failed to add (LuaScriptComponent) component to GameObject");
					} else {
						RPG::Log("SceneMain", "Added (" + component->Name() + ") component to GameObject");
					}
				}
				if (ImGui::Selectable("Mesh Component")) {
					std::shared_ptr<RPG::IComponent> component = selectedGameObject->AddComponent(std::make_unique<RPG::MeshComponent>(RPG::MeshComponent(RPG::Assets::StaticMesh::Crate, RPG::Assets::Texture::Crate)));
					if (component == nullptr) {
						RPG::Log("SceneMain", "Failed to add (MeshComponent) component to GameObject");
					} else {
						RPG::Log("SceneMain", "Added (" + component->Name() + ") component to GameObject");
					}
				}
				if (ImGui::Selectable("Sprite Component")) {
					std::shared_ptr<RPG::IComponent> component = selectedGameObject->AddComponent(std::make_unique<RPG::SpriteComponent>(RPG::SpriteComponent(RPG::Assets::Texture::Crate)));
					if (component == nullptr) {
						RPG::Log("SceneMain", "Failed to add (SpriteComponent) component to GameObject");
					} else {
						RPG::Log("SceneMain", "Added (" + component->Name() + ") component to GameObject");
					}
				}
				if (ImGui::Selectable("Box Collider Component")) {
					std::shared_ptr<RPG::IComponent> component = selectedGameObject->AddComponent(std::make_unique<RPG::BoxColliderComponent>(RPG::BoxColliderComponent(glm::vec3{1, 1, 1}, false)));
					if (component == nullptr) {
						RPG::Log("SceneMain", "Failed to add (Box Collider) component to GameObject");
					} else {
						RPG::Log("SceneMain", "Added (" + component->Name() + ") component to GameObject");
					}
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

	void RenderProperty(std::shared_ptr<RPG::Property> property) {
		//Perhaps have a list that we can add new types to that contain an Action<> this makes it easier to add new types in the future
		for (auto const& [key, value] : RPG::Serializer::GetInstance().GetPropertyLayouts()) {
			if (key == property->GetType()) {
				value(property);
				return;
			}
		}

		RPG::Log("Inspector", "Unable to render property with type (" + property->GetType() + ")");
	}

	void BuildDefaultPropertyLayouts() {
		RPG::Serializer::GetInstance().AddPropertyLayout({"std::string", [](std::shared_ptr<RPG::Property> property) {
			std::any prop = property->GetProperty();
			std::string v = std::any_cast<std::string>(prop);
			if (ImGui::InputText(property->GetEditorName().c_str(), &v)) {
				property->SetProperty(v);
			}
		}});

		RPG::Serializer::GetInstance().AddPropertyLayout({"int", [](std::shared_ptr<RPG::Property> property) {
			std::any prop = property->GetProperty();
			int v = std::any_cast<int>(prop);
			if (ImGui::DragInt(property->GetEditorName().c_str(), (int*)&v)) {
				property->SetProperty(v);
			}
		}});

		RPG::Serializer::GetInstance().AddPropertyLayout({"float", [](std::shared_ptr<RPG::Property> property) {
			std::any prop = property->GetProperty();
			float v = std::any_cast<float>(prop);
			if (ImGui::DragFloat(property->GetEditorName().c_str(), (float*)&v)) {
				property->SetProperty(v);
			}
		}});

		RPG::Serializer::GetInstance().AddPropertyLayout({"bool", [](std::shared_ptr<RPG::Property> property) {
			std::any prop = property->GetProperty();
			bool v = std::any_cast<bool>(prop);
			if (ImGui::Checkbox(property->GetEditorName().c_str(), (bool*)&v)) {
				property->SetProperty(v);
			}
		}});

		RPG::Serializer::GetInstance().AddPropertyLayout({"glm::vec2", [](std::shared_ptr<RPG::Property> property) {
			std::any prop = property->GetProperty();
			glm::vec2 v = std::any_cast<glm::vec2>(prop);
			if (ImGui::DragFloat2(property->GetEditorName().c_str(), (float*)&v)) {
				property->SetProperty(v);
			}
		}});

		RPG::Serializer::GetInstance().AddPropertyLayout({"glm::vec3", [](std::shared_ptr<RPG::Property> property) {
			std::any prop = property->GetProperty();
			glm::vec3 v = std::any_cast<glm::vec3>(prop);
			if (ImGui::DragFloat3(property->GetEditorName().c_str(), (float*)&v)) {
				property->SetProperty(v);
			}
		}});

		RPG::Serializer::GetInstance().AddPropertyLayout({"RPG::CameraType", [](std::shared_ptr<RPG::Property> property) {
			std::any prop = property->GetProperty();
			RPG::CameraType v = std::any_cast<RPG::CameraType>(prop);
			int num = static_cast<int>(v);
			if (ImGui::Combo(property->GetEditorName().c_str(), &num, "Perspective\0Orthographic\0\0")) {
				v = static_cast<RPG::CameraType>(num);
				property->SetProperty(v);
			}
		}});

		RPG::Serializer::GetInstance().AddPropertyLayout({"RPG::Assets::StaticMesh", [](std::shared_ptr<RPG::Property> property) {
			std::any prop = property->GetProperty();
			RPG::Assets::StaticMesh v = std::any_cast<RPG::Assets::StaticMesh>(prop);
			int num = static_cast<int>(v);
			if (ImGui::Combo(property->GetEditorName().c_str(), &num, "Quad\0Crate\0Cube\0\0")) {
				v = static_cast<RPG::Assets::StaticMesh>(num);
				property->SetProperty(v);
			}
		}});

		RPG::Serializer::GetInstance().AddPropertyLayout({"RPG::Assets::Texture", [](std::shared_ptr<RPG::Property> property) {
			std::any prop = property->GetProperty();
			RPG::Assets::Texture v = std::any_cast<RPG::Assets::Texture>(prop);
			int num = static_cast<int>(v);
			if (ImGui::Combo(property->GetEditorName().c_str(), &num, "Crate\0Sprite\0Default\0Collider\0Trigger\0Water\0\0")) {
				v = static_cast<RPG::Assets::Texture>(num);
				property->SetProperty(v);
			}
		}});
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