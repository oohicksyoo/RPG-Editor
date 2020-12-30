//
// Created by Alex on 12/20/2020.
//

#pragma once

#include "IEditorWindow.hpp"
#include "ImGUIWrapper.hpp"
#include "../engine/core/InternalPointer.hpp"
#include "../engine/core/Action.hpp"
#include "MainMenuBarEditorWindowToggle.hpp"

namespace RPG {
	struct MainMenuBarWindow : public IEditorWindow {
		public:
			MainMenuBarWindow();
			void Render(ImGuiID dockID) override;
			RPG::Action<>::Callback ToggleIsOpen() override;
			RPG::Action<>::Func<bool> IsOpen() override;
			void AddToggleableEditorWindow(RPG::MainMenuBarEditorWindowToggle editorWindow);


			void PlayToggleAction(RPG::Action<>::Callback callback);
			void PlayToggleFunc(RPG::Action<>::Func<bool> callback);

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


