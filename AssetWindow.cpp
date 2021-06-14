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
#include "payloads/ModelPayload.hpp"
#include "payloads/GeneralPayload.hpp"
#include "EditorStats.hpp"

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

            if (entry.path().has_extension() && entry.path().extension().string() == ".ts")
                continue;

			if (entry.is_directory() && entry.path().stem().string() == "node_modules")
			    continue;

			if (entry.path().stem().string() == "package" && entry.path().has_extension() && entry.path().extension().string() == ".json")
                continue;

            if (entry.path().stem().string() == "package-lock" && entry.path().has_extension() && entry.path().extension().string() == ".json")
                continue;

            if (entry.path().stem().string() == "tsconfig" && entry.path().has_extension() && entry.path().extension().string() == ".json")
                continue;

            if (entry.path().stem().string() == "_help" && entry.path().has_extension() && entry.path().extension().string() == ".txt")
                continue;

			flags = ImGuiTreeNodeFlags_None;
			if (entry.is_directory()) {
				flags |= ImGuiTreeNodeFlags_OpenOnArrow;
			} else {
				flags |= ImGuiTreeNodeFlags_Leaf;
			}

			std::string labelName = entry.path().string() + "##" + entry.path().string();
 			bool isOpened = ImGui::TreeNodeEx(labelName.c_str(), flags, entry.path().filename().string().c_str());

			if (!entry.is_directory() && entry.path().has_extension()) {
				if (entry.path().extension().string() == ".obj") {
					//Drag - payload Model
					if (ImGui::BeginDragDropSource()) {
						RPG::GeneralPayload payload = RPG::GeneralPayload();
						payload.path = FixFilePath(entry.path().string());

						RPG::EditorStats::GetInstance().SetPayload(payload);

						ImGui::SetDragDropPayload("Model", NULL, 0);
						std::string str = "Model: " + payload.path;
						ImGui::Text(str.c_str());
						ImGui::EndDragDropSource();
					}
				} else if (entry.path().extension().string() == ".png") {
					//Drag - payload Texture
					if (ImGui::BeginDragDropSource()) {
						RPG::GeneralPayload payload = RPG::GeneralPayload();
						payload.path = FixFilePath(entry.path().string());

						RPG::EditorStats::GetInstance().SetPayload(payload);

						ImGui::SetDragDropPayload("Texture", NULL, 0);
						std::string str = "Texture: " + payload.path;
						ImGui::Text(str.c_str());
						ImGui::EndDragDropSource();
					}
				} else if (entry.path().extension().string() == ".scene") {
					//Drag - payload Scene
					if (ImGui::BeginDragDropSource()) {
						RPG::GeneralPayload payload = RPG::GeneralPayload();
						payload.path = FixFilePath(entry.path().string());

						RPG::EditorStats::GetInstance().SetPayload(payload);

						ImGui::SetDragDropPayload("Scene", NULL, 0);
						std::string str = "Scene: " + payload.path;
						ImGui::Text(str.c_str());
						ImGui::EndDragDropSource();
					}
				} else if (entry.path().extension().string() == ".lua") {
					//Drag - payload Lua
					if (ImGui::BeginDragDropSource()) {
						RPG::GeneralPayload payload = RPG::GeneralPayload();
						payload.path = FixFilePath(entry.path().string());

						RPG::EditorStats::GetInstance().SetPayload(payload);

						ImGui::SetDragDropPayload("Lua", NULL, 0);
						std::string str = "Lua: " + payload.path;
						ImGui::Text(str.c_str());
						ImGui::EndDragDropSource();
					}
				}
			}

			if (isOpened) {
				if (entry.is_directory()) {
					RenderDirectory(entry.path().string());
				}
				ImGui::TreePop();
			}
		}
	}

	std::string FixFilePath(std::string value) {
		for (int i = 0; i < value.length(); ++i) {
			if (value[i] == '\\') {
				value[i] = '/';
			}
		}
		return value;
	}

	void PreformSync() {
		json j = json::object();
		auto models = json::array();
		auto textures = json::array();
		for (auto entry : fs::directory_iterator("assets/models")) {
			if (entry.path().has_extension() && entry.path().extension().string() != ".obj")
				continue;

			std::string s = FixFilePath(entry.path().string());
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
