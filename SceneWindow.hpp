//
// Created by Alex on 12/20/2020.
//

#pragma once

#include "IEditorWindow.hpp"
#include "ImGUIWrapper.hpp"
#include "../engine/core/InternalPointer.hpp"
#include "../engine/core/Action.hpp"
#include "../engine/core/FrameBuffer.hpp"
#include "../engine/core/GameObject.hpp"

namespace RPG {
	struct SceneWindow : public IEditorWindow {
		public:
			SceneWindow();
			void Render(ImGuiID dockID) override;
			RPG::Action<>::Callback ToggleIsOpen() override;
			RPG::Action<>::Func<bool> IsOpen() override;
			void SetFrameBuffer(uint32_t frameBufferID);
			void SetSelectedGameObject(std::shared_ptr<RPG::GameObject> gameObject = nullptr);
            void SelectedGameObjectAction(RPG::Action<std::shared_ptr<RPG::GameObject>>::Callback callback);

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


