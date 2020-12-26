//
// Created by Alex on 12/20/2020.
//

#pragma once

#include "IEditorWindow.hpp"
#include "ImGUIWrapper.hpp"
#include "../engine/core/InternalPointer.hpp"
#include "../engine/core/Action.hpp"
#include "../engine/core/FrameBuffer.hpp"

namespace RPG {
	struct SceneWindow : public IEditorWindow {
		public:
			SceneWindow();
			void Render(ImGuiID dockID) override;
			RPG::Action<>::Callback ToggleIsOpen() override;
			RPG::Action<>::Func<bool> IsOpen() override;
			void SetFrameBuffer(uint32_t frameBufferID);

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


