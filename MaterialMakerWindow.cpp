//
// Created by Alex on 7/29/2021.
//

#include "MaterialMakerWindow.hpp"
#include "../engine/core/components/MeshComponent.hpp"
#include "../engine/core/Property.hpp"
#include "payloads/GeneralPayload.hpp"
#include "EditorStats.hpp"
#include "../engine/core/Serializer.hpp"

using RPG::MaterialMakerWindow;

struct MaterialMakerWindow::Internal {
    bool isOpened;
    std::shared_ptr<RPG::Property> property;
    std::string newMaterialName;
    std::shared_ptr<RPG::FrameBuffer> frameBuffer;

    Internal() : isOpened(true),
                 property(std::make_unique<RPG::Property>(std::string(""), "Material", "RPG::Resource::String", true, "Material")) {}

    void SubmitFramebuffer(std::shared_ptr<RPG::FrameBuffer> frameBuffer) {
        this->frameBuffer = frameBuffer;
    }

    void Render(ImGuiID dockID) {
        if (!isOpened) return;

        ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
        ImGui::Begin("Material Maker", &isOpened);

        std::any prop = property->GetProperty();
        std::string v = std::any_cast<std::string>(prop);
        bool hasMaterialLoaded = v.length() > 0;

        if (!hasMaterialLoaded) {
            ImGui::InputText("##MaterialName", &newMaterialName);
            ImGui::SameLine();
            if (newMaterialName.length() == 0) {
                ImGui::Text("Create");
            } else {
                if (ImGui::Button("Create")) {
                    std::string path = "assets/materials/" + newMaterialName + ".mat";
                    RPG::Serializer::GetInstance().SaveMaterial(std::make_unique<RPG::Material>(newMaterialName), path);
                    newMaterialName = "";
                    //Set and reload the property so we can display the material to the user
                    property->SetProperty(std::string(path));
                    prop = property->GetProperty();
                    v = std::any_cast<std::string>(prop);
                }
            }
        } else {
            if (ImGui::Button("Unload")) {
                newMaterialName = "";
                property->SetProperty(std::string(""));
            }

            //TODO: Render material in a scene
        }

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75, 0.75, 0.75, 1));
        std::string id = "##" + property->GetGuid();
        ImGui::InputText(id.c_str(), &v, ImGuiInputTextFlags_ReadOnly);
        ImGui::PopStyleColor();

        if (ImGui::IsItemHovered() && hasMaterialLoaded) {
            ImGui::BeginTooltip();
            ImGui::Text(v.c_str());
            ImGui::EndTooltip();
        }

        //Dropable Payloads
        if (property->AllowsDragAndDrop()) {
            //Drop - payload
            if (ImGui::BeginDragDropTarget()) {
                if (ImGui::AcceptDragDropPayload(property->DragAndDropTag().c_str())) {
                    RPG::GeneralPayload p = RPG::EditorStats::GetInstance().GetPayload();
                    RPG::Log("Payload", p.path);
                    property->SetProperty(p.path);
                }
                ImGui::EndDragDropTarget();
            }
        }

        ImGui::SameLine();
        ImGui::Text(property->GetName().c_str(), property->GetEditorName().c_str());

        if (hasMaterialLoaded) {
            ImGui::Image((void *) (intptr_t) frameBuffer->GetRenderTextureID(), ImVec2{16 * 10, 9 * 10}, ImVec2{0, 1}, ImVec2{1, 0});
        }

        ImGui::End();
    }
};

MaterialMakerWindow::MaterialMakerWindow() : internal(RPG::MakeInternalPointer<Internal>()) {}

void MaterialMakerWindow::Render(ImGuiID dockID) {
    internal->Render(dockID);
}

RPG::Action<>::Callback MaterialMakerWindow::ToggleIsOpen() {
    return [this] () {
        internal->isOpened = !internal->isOpened;
    };
}

RPG::Action<>::Func<bool> MaterialMakerWindow::IsOpen() {
    return [this] () -> bool {
        return internal->isOpened;
    };
}

void MaterialMakerWindow::SubmitFramebuffer(std::shared_ptr<RPG::FrameBuffer> frameBuffer) {
    internal->SubmitFramebuffer(frameBuffer);
}