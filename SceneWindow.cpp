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
#include "../engine/core/Input/InputManager.hpp"

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

		auto windowPosition = ImGui::GetWindowPos();
		auto windowSize = ImGui::GetWindowSize();
		RPG::EditorStats::GetInstance().SetSceneScreenRect(glm::vec4{windowPosition.x, windowPosition.y, windowSize.x, windowSize.y});

		ImVec2 size = ImGui::GetContentRegionAvail();
		ImVec2 contentSize = CalculateContentSize(size);
		ImGui::Image((void *) (intptr_t) frameBufferID, contentSize, ImVec2{0, 1}, ImVec2{1, 0});

		std::shared_ptr<RPG::CameraComponent> camera = RPG::SceneManager::GetInstance().GetCurrentScene()->GetCamera();

		int gizmoTool = RPG::EditorStats::GetInstance().GetGizmoTool();

		// Yellow is content region min/max
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax;

		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x = vMin.x + contentSize.x;
		vMax.y = vMin.y + contentSize.y;

		//For Testing
		//ImGui::GetForegroundDrawList()->AddRect( vMin, vMax, IM_COL32( 255, 255, 0, 255 ) );

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(vMin.x, vMin.y, contentSize.x, contentSize.y);

		if (selectedGameObject != nullptr && gizmoTool != -1) {
			auto transform = selectedGameObject->GetTransform();
			glm::mat4 modelMatrix = transform->GetTransformMatrix();

			bool snap = RPG::InputManager::GetInstance().IsKeyDown(RPG::Input::Key::LeftAlt);
			float snapValue = 0.5f;


			if (gizmoTool == ImGuizmo::OPERATION::ROTATE) {
				snapValue = 45.0f;
			}

			float snapValues[3] = { snapValue, snapValue, snapValue };

			float tmpMatrix[16];
			ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(transform->GetPosition()), glm::value_ptr(transform->GetRotation()), glm::value_ptr(transform->GetScale()), tmpMatrix);
			ImGuizmo::Manipulate(glm::value_ptr(camera->GetViewMatrix()), glm::value_ptr(camera->GetProjectionMatrix()), (ImGuizmo::OPERATION)gizmoTool, ImGuizmo::MODE::LOCAL, tmpMatrix);

			if (ImGuizmo::IsUsing()) {
				auto transform = selectedGameObject->GetTransform();
				float matrixTranslation[3], matrixRotation[3], matrixScale[3];
				ImGuizmo::DecomposeMatrixToComponents(tmpMatrix, matrixTranslation, matrixRotation, matrixScale);

				switch (gizmoTool) {
					case ImGuizmo::OPERATION::TRANSLATE:
						transform->SetPosition(glm::vec3{matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]});
						break;
					case ImGuizmo::OPERATION::ROTATE:
						transform->SetRotation(glm::vec3{matrixRotation[0], matrixRotation[1], matrixRotation[2]});
						break;
					case ImGuizmo::OPERATION::SCALE:
						transform->SetScale(glm::vec3{matrixScale[0], matrixScale[1], matrixScale[2]});
						break;
					default:
						break;
				}
			}
		}

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