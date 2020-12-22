//
// Created by Alex on 12/22/2020.
//

#pragma once

#include "../engine/core/Action.hpp"
#include <string>

namespace RPG {
	struct MainMenuBarEditorWindowToggle {
		public:
			std::string windowName;
			RPG::Action<>::Callback onToggleAction;
			RPG::Action<>::Func<bool> onIsOpenFunc;
	};
}
