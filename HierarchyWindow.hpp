//
// Created by Alex on 12/22/2020.
//

#pragma once

#include "IEditorWindow.hpp"
#include "ImGUIWrapper.hpp"
#include "../engine/core/InternalPointer.hpp"

namespace RPG {
	struct HierarchyWindow : public IEditorWindow {
		public:
			HierarchyWindow(bool isOpened);
			void Render(ImGuiID dockID) override;
			RPG::Action<>::Callback ToggleIsOpen() override;
			RPG::Action<>::Func<bool> IsOpen() override;

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


