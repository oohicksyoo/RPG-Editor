//
// Created by Alex on 1/8/2021.
//

#include "AssetWindow.hpp"
#include <string>
#include <iostream>
#include <filesystem>

using RPG::AssetWindow;
namespace fs = std::filesystem;

struct AssetWindow::Internal {
	bool isOpened;

	Internal() : isOpened(true) {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Assets", &isOpened);

		RenderDirectory("assets");

		ImGui::End();
	}

	void RenderDirectory(std::string path) {
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;

		for (auto entry : fs::directory_iterator(path)) {
			if (entry.path().has_extension() && entry.path().extension().string() == ".md")
				continue;

			flags = ImGuiTreeNodeFlags_None;
			if (entry.is_directory()) {
				flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			} else {
				flags |= ImGuiTreeNodeFlags_Leaf;
			}

			bool isOpened = ImGui::TreeNodeEx(entry.path().string().c_str(), flags, entry.path().filename().string().c_str());

			if (isOpened) {
				if (entry.is_directory()) {
					RenderDirectory(entry.path().string());
				}
				ImGui::TreePop();
			}
		}
	}
};

AssetWindow::AssetWindow() : internal(RPG::MakeInternalPointer<Internal>()) {}

void AssetWindow::Render(ImGuiID dockID) {
	internal->Render(dockID);
}

RPG::Action<>::Callback AssetWindow::ToggleIsOpen() {
	return [this] () {
		internal->isOpened = !internal->isOpened;
	};
}

RPG::Action<>::Func<bool> AssetWindow::IsOpen() {
	return [this] () -> bool {
		return internal->isOpened;
	};
}
