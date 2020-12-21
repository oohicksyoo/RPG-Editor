//
// Created by Alex on 12/20/2020.
//

#pragma once

#include "ImGUIWrapper.hpp"

namespace RPG {
	struct IEditorWindow {
		public:
			IEditorWindow() = default;
			virtual ~IEditorWindow() = default;
			virtual void Render(ImGuiID dockID) = 0;
	};
}
