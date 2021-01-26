//
// Created by Alex on 1/8/2021.
//

#include "AssetWindow.hpp"
#include "../engine/core/Log.hpp"
#include "../engine/core/Content.hpp"
#include "../engine/core/Mesh.hpp"
#include <string>
#include <iostream>
#include <filesystem>
#include <regex>
#include <nlohmann/json.hpp>

using RPG::AssetWindow;
using json = nlohmann::json;
namespace fs = std::filesystem;

struct AssetWindow::Internal {
	bool isOpened;

	Internal() : isOpened(true) {}

	void Render(ImGuiID dockID) {
		if (!isOpened) return;

		ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
		ImGui::Begin("Assets", &isOpened);

		if (ImGui::Button("Sync")) {
			PreformSync();
		}

		ImGui::Spacing();

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

	void PreformSync() {
		json j = json::object();
		auto models = json::array();
		auto textures = json::array();
		for (auto entry : fs::directory_iterator("assets/models")) {
			if (entry.path().has_extension() && entry.path().extension().string() != ".obj")
				continue;

			std::string s = entry.path().string();
			for (int i = 0; i < s.length(); ++i) {
				if (s[i] == '\\') {
					s[i] = '/';
				}
			}

			RPG::Content::GetInstance().Load<RPG::Mesh>(s);
			models.push_back(s);
		}
		j["Models"] = models;

		for (auto entry : fs::directory_iterator("assets/textures")) {
			if (entry.path().has_extension() && entry.path().extension().string() != ".png")
				continue;

			std::string s = entry.path().string();
			for (int i = 0; i < s.length(); ++i) {
				if (s[i] == '\\') {
					s[i] = '/';
				}
			}

			RPG::Content::GetInstance().Load<RPG::Texture>(s);
			textures.push_back(s);
		}
		j["Textures"] = textures;
		RPG::Assets::SaveTextFile(j.dump(), "assets/project/resources.projectasset");
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
