//
// Created by Alex on 7/29/2021.
//

#pragma once

#include "IEditorWindow.hpp"
#include "ImGUIWrapper.hpp"
#include "../engine/core/InternalPointer.hpp"
#include "../engine/core/GameObject.hpp"
#include "../engine/core/FrameBuffer.hpp"
#include <memory>

namespace RPG {
    struct MaterialMakerWindow : public IEditorWindow {
        public:
            MaterialMakerWindow();
            void Render(ImGuiID dockID) override;
            RPG::Action<>::Callback ToggleIsOpen() override;
            RPG::Action<>::Func<bool> IsOpen() override;
            void SubmitFramebuffer(std::shared_ptr<RPG::FrameBuffer> frameBuffer);
            std::shared_ptr<RPG::Material> GetCurrentMaterial();

        private:
            struct Internal;
            RPG::InternalPointer<Internal> internal;
    };
}