//
// Created by Alex on 12/22/2020.
//

#pragma once

#include "IEditorWindow.hpp"
#include "ImGUIWrapper.hpp"
#include "../engine/core/InternalPointer.hpp"
#include "../engine/core/Hierarchy.hpp"

namespace RPG {
	struct HierarchyWindow : public IEditorWindow {
		public:
			HierarchyWindow();
			void Render(ImGuiID dockID) override;
			RPG::Action<>::Callback ToggleIsOpen() override;
			RPG::Action<>::Func<bool> IsOpen() override;
			void SetHierarchy(std::shared_ptr<RPG::Hierarchy> hierarchy);

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


