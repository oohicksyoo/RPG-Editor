//
// Created by Alex on 1/16/2021.
//

#include "InputHelperWindow.hpp"
#include "../engine/core/input/InputManager.hpp"
#include "../engine/core/Time.hpp"

using RPG::InputHelperWindow;

struct InputHelperWindow::Internal {
	bool isOpened;

	Internal() : isOpened(true) {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Input Helper", &isOpened);

		bool isOpen = ImGui::TreeNodeEx("Current State", ImGuiTreeNodeFlags_OpenOnArrow);

		if (isOpen) {
			auto state = RPG::InputManager::GetInstance().GetState();
			if (ImGui::TreeNodeEx("Keyboard", ImGuiTreeNodeFlags_OpenOnArrow)) {
				for (int i = 0; i < RPG::Input::MAX_KEYBOARD_KEYS; ++i) {
					bool isPressed = state.keyboard.pressed[i];
					bool isDown = state.keyboard.down[i];
					bool isReleased = state.keyboard.released[i];
					uint64_t timeStamp = state.keyboard.timestamp[i];
					uint64_t heldSeconds = (timeStamp == 0) ? 0 : RPG::Time::milliseconds - timeStamp;
					float hs = heldSeconds / 10000000.0f;
					std::string keyName = RPG::InputManager::GetInstance().GetNameOf((RPG::Input::Key)i);

					if (keyName != "Unknown") {
						std::string str = keyName + ": Pressed-" + (isPressed ? "True" : "False") + " Down-" +
										  (isDown ? "True" : "False") + " Released-" + (isReleased ? "True" : "False") +
										  " Time-" + std::to_string(hs);
						ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf);
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Mouse", ImGuiTreeNodeFlags_OpenOnArrow)) {
				for (int i = 0; i < RPG::Input::MAX_MOUSE_BUTTONS; ++i) {
					bool isPressed = state.mouse.pressed[i];
					bool isDown = state.mouse.down[i];
					bool isReleased = state.mouse.released[i];
					uint64_t timeStamp = state.mouse.timestamp[i];
					uint64_t heldSeconds = (timeStamp == 0) ? 0 : RPG::Time::milliseconds - timeStamp;
					float hs = heldSeconds / 10000000.0f;
					std::string keyName = RPG::InputManager::GetInstance().GetNameOf((RPG::Input::MouseButton)i);

					if (keyName != "Unknown") {
						std::string str = keyName + ": Pressed-" + (isPressed ? "True" : "False") + " Down-" +
										  (isDown ? "True" : "False") + " Released-" + (isReleased ? "True" : "False") +
										  " Time-" + std::to_string(hs);
						ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf);
						ImGui::TreePop();
					}
				}

				std::string mouseWheel = "Mouse Wheel-(" + std::to_string(state.mouse.wheel.x) + "," + std::to_string(state.mouse.wheel.y) + ")";
				ImGui::TreeNodeEx(mouseWheel.c_str(), ImGuiTreeNodeFlags_Leaf);
				ImGui::TreePop();

				std::string mousePosition = "Mouse Position-(" + std::to_string(state.mouse.position.x) + "," + std::to_string(state.mouse.position.y) + ")";
				ImGui::TreeNodeEx(mousePosition.c_str(), ImGuiTreeNodeFlags_Leaf);
				ImGui::TreePop();

				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Controllers", ImGuiTreeNodeFlags_OpenOnArrow)) {
				for (int i = 0; i < RPG::Input::MAX_CONTROLLERS; ++i) {
					std::string controllerID = "Controller " + std::to_string(i) + "##Controller" + std::to_string(i);
					if (ImGui::TreeNodeEx(controllerID.c_str(), ImGuiTreeNodeFlags_OpenOnArrow)) {

						if (!state.controllers[i].isConnected) {
							ImGui::Text("Not Connected");
						} else {
							for (int j = 0; j < RPG::Input::MAX_CONTROLLER_BUTTONS; j++) {
								bool isPressed = state.controllers[i].pressed[j];
								bool isDown = state.controllers[i].down[j];
								bool isReleased = state.controllers[i].released[j];
								uint64_t timeStamp = state.controllers[i].buttonTimestamp[j];
								uint64_t heldSeconds = (timeStamp == 0) ? 0 : RPG::Time::milliseconds - timeStamp;
								float hs = heldSeconds / 10000000.0f;
								std::string keyName = RPG::InputManager::GetInstance().GetNameOf(
										(RPG::Input::ControllerButton) j);

								if (keyName != "Unknown") {
									std::string str =
											keyName + ": Pressed-" + (isPressed ? "True" : "False") + " Down-" +
											(isDown ? "True" : "False") + " Released-" +
											(isReleased ? "True" : "False") +
											" Time-" + std::to_string(hs);
									ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf);
									ImGui::TreePop();
								}
							}

							ImGui::Spacing();

							for (int k = 0; k < Input::MAX_CONTROLLER_AXIS; ++k) {
								float axis = state.controllers[i].axis[k];
								uint64_t timeStamp = state.controllers[i].axisTimestamp[k];
								uint64_t heldSeconds = (timeStamp == 0) ? 0 : RPG::Time::milliseconds - timeStamp;
								float hs = heldSeconds / 10000000.0f;
								std::string keyName = RPG::InputManager::GetInstance().GetNameOf(
										(RPG::Input::ControllerAxis) k);

								if (keyName != "Unknown") {
									std::string str = keyName + ": Axis-" + std::to_string(axis) +
													  " Time-" + std::to_string(hs);
									ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf);
									ImGui::TreePop();
								}
							}
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}


		ImGui::End();
	}
};

InputHelperWindow::InputHelperWindow() : internal(RPG::MakeInternalPointer<Internal>()) {}

void InputHelperWindow::Render(ImGuiID dockID) {
	internal->Render(dockID);
}

RPG::Action<>::Callback InputHelperWindow::ToggleIsOpen() {
	return [this] () {
		internal->isOpened = !internal->isOpened;
	};
}

RPG::Action<>::Func<bool> InputHelperWindow::IsOpen() {
	return [this] () -> bool {
		return internal->isOpened;
	};
}