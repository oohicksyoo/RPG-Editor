//
// Created by Alex on 12/20/2020.
//

#include "SceneWindow.hpp"
#include "../engine/application/ApplicationStats.hpp"
#include "ImGuizmoWrapper.hpp"
#include "../engine/core/SceneManager.hpp"
#include "../engine/core/GLMWrapper.hpp"
#include "EditorStats.hpp"
#include "../engine/core/Log.hpp"
#include "payloads/ModelPayload.hpp"
#include "payloads/GeneralPayload.hpp"

using RPG::SceneWindow;

struct SceneWindow::Internal {
	uint32_t frameBufferID;
	bool isOpened = true;
	glm::mat4 identity;
	std::shared_ptr<RPG::GameObject> selectedGameObject;


	Internal() : identity(glm::mat4{1.0f}),
				 selectedGameObject(nullptr) {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Scene", &isOpened);

		ImVec2 size = ImGui::GetContentRegionAvail();
		ImVec2 contentSize = CalculateContentSize(size);
		ImGui::Image((void *) (intptr_t) frameBufferID, contentSize, ImVec2{0, 1}, ImVec2{1, 0});

		std::shared_ptr<RPG::CameraComponent> camera = RPG::SceneManager::GetInstance().GetCurrentScene()->GetCamera();
		glm::vec3 cameraPosition = RPG::SceneManager::GetInstance().GetCurrentScene()->GetCameraPosition();

		int gizmoTool = RPG::EditorStats::GetInstance().GetGizmoTool();

		if (selectedGameObject != nullptr && gizmoTool != -1) {
			glm::mat4 modelMatrix = selectedGameObject->GetTransform()->GetTransformMatrix();

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, contentSize.x, contentSize.y);

			bool snap = true;//Left Control
			float snapValue = 0.5f;


			if (gizmoTool == ImGuizmo::OPERATION::ROTATE) {
				snapValue = 45.0f;
			}

			float snapValues[3] = { snapValue, snapValue, snapValue };
			ImGuizmo::Manipulate(glm::value_ptr(camera->GetViewMatrix(cameraPosition)), glm::value_ptr(camera->GetProjectionMatrix()),
								 (ImGuizmo::OPERATION)gizmoTool, ImGuizmo::LOCAL, glm::value_ptr(modelMatrix),
								 nullptr, (snap) ? snapValues : nullptr);

			if (ImGuizmo::IsUsing()) {
				auto transform = selectedGameObject->GetTransform();
				glm::vec3 translation, rotation, scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(modelMatrix), glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));

				glm::vec3 currentRotation = transform->GetRotation();
				glm::vec3 deltaRotation = rotation - currentRotation;

				transform->SetPosition(translation);
				transform->SetRotation(currentRotation + deltaRotation);
				transform->SetScale(scale);
			}
		}

		//ImGuizmo::ViewManipulate(&(camera->GetViewMatrix(cameraPosition))[0][0], 10.0f, ImVec2(ImGui::GetWindowPos().x + contentSize.x - 128, ImGui::GetWindowPos().y), ImVec2(128, 128), 0x10101010);


		//Drop - payload Model
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Scene")) {
				RPG::GeneralPayload p = RPG::EditorStats::GetInstance().GetPayload();
				RPG::Log("Payload", p.path);
				RPG::SceneManager::GetInstance().LoadScene(p.path);
			}
			ImGui::EndDragDropTarget();
		}

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

void SceneWindow::SetSelectedGameObject(std::shared_ptr<RPG::GameObject> gameObject) {
	internal->selectedGameObject = gameObject;
}