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
