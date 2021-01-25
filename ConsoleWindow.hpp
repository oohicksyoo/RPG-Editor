//
// Created by Alex on 1/25/2021.
//

#pragma once
#include "IEditorWindow.hpp"
#include "../engine/core/InternalPointer.hpp"

namespace RPG {
	struct ConsoleWindow : public IEditorWindow {
		public:
			ConsoleWindow();
			void Render(ImGuiID dockID) override;
			RPG::Action<>::Callback ToggleIsOpen() override;
			RPG::Action<>::Func<bool> IsOpen() override;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


