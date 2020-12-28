//
// Created by Alex on 12/20/2020.
//

#pragma once

#include "../engine/core/InternalPointer.hpp"
#include "../engine/core/SDLWindow.hpp"
#include "../engine/core/FrameBuffer.hpp"
#include "../engine/core/Hierarchy.hpp"

namespace RPG {
	struct EditorManager {
		public:
			EditorManager(const RPG::SDLWindow& window, SDL_GLContext context);
			void NewFrame(const RPG::SDLWindow& window);
			void BuildGUI(std::shared_ptr<RPG::FrameBuffer> frameBuffer, std::shared_ptr<RPG::Hierarchy> hierarchy);
			void Render();

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


