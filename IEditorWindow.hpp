//
// Created by Alex on 12/20/2020.
//

#pragma once

#include "ImGUIWrapper.hpp"
#include "../engine/core/Action.hpp"

namespace RPG {
	struct IEditorWindow {
		public:
			IEditorWindow() = default;
			virtual ~IEditorWindow() = default;
			virtual void Render(ImGuiID dockID) = 0;
			virtual RPG::Action<>::Callback ToggleIsOpen() = 0;
			virtual RPG::Action<>::Func<bool> IsOpen() = 0;
	};
}
