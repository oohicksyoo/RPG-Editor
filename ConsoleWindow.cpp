//
// Created by Alex on 1/25/2021.
//

#include "ConsoleWindow.hpp"
#include "EditorStats.hpp"
#include <string>

using RPG::ConsoleWindow;

struct ConsoleWindow::Internal {
	bool isOpened;

	Internal() : isOpened(true) {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Console", &isOpened);

		for (auto log : RPG::EditorStats::GetInstance().GetLogs()) {
			std::string s = "[" + log.caller + "] " + log.message;
			ImGui::Text(s.c_str());
		}

		ImGui::End();
	}
};

ConsoleWindow::ConsoleWindow() : internal(RPG::MakeInternalPointer<Internal>()) {}

void ConsoleWindow::Render(ImGuiID dockID) {
	internal->Render(dockID);
}

RPG::Action<>::Callback ConsoleWindow::ToggleIsOpen() {
	return [this] () {
		internal->isOpened = !internal->isOpened;
	};
}

RPG::Action<>::Func<bool> ConsoleWindow::IsOpen() {
	return [this] () -> bool {
		return internal->isOpened;
	};
}
