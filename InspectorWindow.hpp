//
// Created by Alex on 12/30/2020.
//

#pragma once

#include "IEditorWindow.hpp"
#include "ImGUIWrapper.hpp"
#include "../engine/core/InternalPointer.hpp"
#include "../engine/core/GameObject.hpp"
#include <memory>

namespace RPG {
	struct InspectorWindow : public IEditorWindow {
		public:
			InspectorWindow();
			void Render(ImGuiID dockID) override;
			RPG::Action<>::Callback ToggleIsOpen() override;
			RPG::Action<>::Func<bool> IsOpen() override;
			void SetSelectedGameObject(std::shared_ptr<RPG::GameObject> gameObject = nullptr);
			void AddPropertyLayout(std::pair<std::string, RPG::Action<std::shared_ptr<RPG::Property>>::Callback> pair);

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


