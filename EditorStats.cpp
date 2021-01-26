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