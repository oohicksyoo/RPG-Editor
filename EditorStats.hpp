//
// Created by Alex on 1/14/2021.
//

#pragma once
#include "../engine/core/Singleton.hpp"
#include "LogFile.hpp"
#include <vector>

namespace RPG {
	struct EditorStats : public Singleton<EditorStats> {
		friend struct Singleton<EditorStats>;

		public:
			EditorStats();
			~EditorStats();
			void SetGizmoTool(int toolIndex);
			int GetGizmoTool();
			void AddLog(RPG::LogFile log);
			std::vector<RPG::LogFile> GetLogs();

		private:
			int gizmoTool = -1;
			std::vector<RPG::LogFile> logs;

	};
}


