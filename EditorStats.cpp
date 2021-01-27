//
// Created by Alex on 1/14/2021.
//

#include "EditorStats.hpp"

using RPG::EditorStats;

EditorStats::EditorStats() {

}

EditorStats::~EditorStats() {

}

int EditorStats::GetGizmoTool() {
	return gizmoTool;
}

void EditorStats::SetGizmoTool(int toolIndex) {
	gizmoTool = toolIndex;
}

void EditorStats::AddLog(RPG::LogFile log) {
	if (logs.size() > 100) {
		logs.erase(logs.begin());
	}
	logs.push_back(log);
}

std::vector<RPG::LogFile> EditorStats::GetLogs() {
	return logs;
}

void EditorStats::SetPayload(RPG::GeneralPayload payload) {
	generalPayload = payload;
}

RPG::GeneralPayload EditorStats::GetPayload() {
	return generalPayload;
}

void EditorStats::SetSceneScreenRect(glm::vec4 rect) {
	sceneScreenRect = rect;
}

bool EditorStats::IsMouseInSceneScreen(glm::vec2 mousePosition) {
	return mousePosition.x >= sceneScreenRect.x && mousePosition.x <= sceneScreenRect.x + sceneScreenRect.z &&
			mousePosition.y >= sceneScreenRect.y && mousePosition.y <= sceneScreenRect.y + sceneScreenRect.w;
}
