//
// Created by Alex on 12/20/2020.
//

#include "SceneWindow.hpp"
#include "../engine/application/ApplicationStats.hpp"
#include "ImGuizmoWrapper.hpp"
#include "../engine/core/SceneManager.hpp"

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

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, contentSize.x, contentSize.y);

		if (selectedGameObject != nullptr) {
			glm::mat4 modelMatrix = selectedGameObject->GetTransform()->GetTransformMatrix();
			if (ImGuizmo::Manipulate(&(camera->GetViewMatrix(cameraPosition))[0][0], &(camera->GetProjectionMatrix())[0][0], ImGuizmo::SCALE, ImGuizmo::LOCAL, &(modelMatrix)[0][0])) {
				auto transform = selectedGameObject->GetTransform();
				float t[3], r[3], s[3];
				ImGuizmo::DecomposeMatrixToComponents(&(modelMatrix)[0][0], t, r, s);
				transform->SetPosition({t[0], t[1], t[2]});
				transform->SetRotation({r[0], r[1], r[2]});
				transform->SetScale({s[0], s[1], s[2]});
			}
		}

		ImGuizmo::ViewManipulate(&(camera->GetViewMatrix(cameraPosition))[0][0], 10.0f, ImVec2(ImGui::GetWindowPos().x + contentSize.x - 128, ImGui::GetWindowPos().y), ImVec2(128, 128), 0x10101010);

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