//
// Created by Alex on 12/20/2020.
//

#pragma once

#include "IEditorWindow.hpp"
#include "ImGUIWrapper.hpp"
#include "../engine/core/InternalPointer.hpp"

namespace RPG {
	struct GameWindow : public IEditorWindow {
		public:
			GameWindow(bool isOpened);
			void Render(ImGuiID dockID) override;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


