//
// Created by Alex on 12/20/2020.
//

#pragma once

#include "../engine/core/InternalPointer.hpp"
#include "../engine/core/SDLWindow.hpp"
#include "../engine/core/FrameBuffer.hpp"
#include "../engine/core/Hierarchy.hpp"

//TODO: Maybe in the future denote a second constructor and flow for a vulkan editor
#include "../engine/application/opengl/OpenGLAssetManager.hpp"

namespace RPG {
	struct EditorManager {
		public:
			EditorManager(const RPG::SDLWindow& window, SDL_GLContext context, std::shared_ptr<RPG::OpenGLAssetManager> assetManager);
			void NewFrame(const RPG::SDLWindow& window);
			void BuildGUI(std::shared_ptr<RPG::FrameBuffer> frameBuffer, std::shared_ptr<RPG::FrameBuffer> gameFrameBuffer, std::shared_ptr<RPG::Hierarchy> hierarchy);
			void Render();
			bool IsGameRunning();
			void OnGeneralEventData(SDL_Event event);
			void SubmitMaterialMakerFrameBuffer(std::shared_ptr<RPG::FrameBuffer> frameBuffer);
			std::shared_ptr<RPG::Material> GetCurrentMaterial();

		private:
			struct Internal;
			RPG::InternalPointer<Internal> internal;
	};
}


