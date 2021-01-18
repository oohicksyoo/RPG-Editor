//
// Created by Alex on 1/14/2021.
//

#pragma once
#include "../engine/core/Singleton.hpp"

namespace RPG {
	struct EditorStats : public Singleton<EditorStats> {
		friend struct Singleton<EditorStats>;

		public:
			EditorStats();
			~EditorStats();
			void SetGizmoTool(int toolIndex);
			int GetGizmoTool();

		private:
			int gizmoTool = -1;
	};
}


