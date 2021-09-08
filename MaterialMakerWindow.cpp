//
// Created by Alex on 7/29/2021.
//

#include "MaterialMakerWindow.hpp"
#include "nlohmann/json.hpp"
#include "../engine/core/components/MeshComponent.hpp"
#include "../engine/core/Property.hpp"
#include "payloads/GeneralPayload.hpp"
#include "EditorStats.hpp"
#include "../engine/core/Serializer.hpp"
#include "../engine/core/Assets.hpp"
#include "../engine/core/Content.hpp"

using json = nlohmann::json;

using RPG::MaterialMakerWindow;

struct MaterialMakerWindow::Internal {
    bool isOpened;
    std::shared_ptr<RPG::OpenGLAssetManager> assetManager;
    std::shared_ptr<RPG::Property> property;
    std::string newMaterialName;
    std::shared_ptr<RPG::FrameBuffer> frameBuffer;
    std::shared_ptr<RPG::Material> material;
    std::vector<std::shared_ptr<RPG::Property>> properties;

    Internal(std::shared_ptr<RPG::OpenGLAssetManager> assetManager) : isOpened(true), assetManager(assetManager),
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
                    material = std::make_unique<RPG::Material>(newMaterialName, 0, "default");
                    RPG::Serializer::GetInstance().SaveMaterial(material, path);
                    newMaterialName = "";
                    //Set and reload the property so we can display the material to the user
                    property->SetProperty(std::string(path));
                    prop = property->GetProperty();
                    v = std::any_cast<std::string>(prop);
                    auto shaderDefinitionFile = RPG::Assets::LoadTextFile("assets/shaders/definitions/" + material->GetShader() + ".shader");
                    GrabProperties(shaderDefinitionFile);
                    material->SetProperties(properties);
                }
            }
        } else {
            if (ImGui::Button("Unload")) {
                newMaterialName = "";
                property->SetProperty(std::string(""));
                material = nullptr;
                properties.clear();
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
                    material = assetManager->GetMaterial(p.path);// std::make_unique<RPG::Material>(RPG::Assets::LoadMaterial(p.path));
                    properties = material->GetProperties();
                    //Save this for when we actually change the Shader file
                    /*//Load Shader Definition file
                    auto shaderDefinitionFile = RPG::Assets::LoadTextFile("assets/shaders/definitions/" + material->GetShader() + ".shader");
                    GrabProperties(shaderDefinitionFile);*/
                }
                ImGui::EndDragDropTarget();
            }
        }

        ImGui::SameLine();
        ImGui::Text(property->GetName().c_str(), property->GetEditorName().c_str());

        if (hasMaterialLoaded && material != nullptr && material->GetName() != "") {
            ImGui::Image((void *) (intptr_t) frameBuffer->GetRenderTextureID(), ImVec2{16 * 10, 9 * 10}, ImVec2{0, 1}, ImVec2{1, 0});

            int renderQueue = material->GetRenderQueue();
            ImGui::InputInt("Render Queue", &renderQueue);
            material->SetRenderQueue(renderQueue);

            auto shader = material->GetShader();
            ImGui::InputText("##MaterialName", &shader);
            material->SetShader(shader);
            ImGui::SameLine();
            if (ImGui::Button("Reload")) {
                properties.clear();
                auto shaderDefinitionFile = RPG::Assets::LoadTextFile("assets/shaders/definitions/" + material->GetShader() + ".shader");
                GrabProperties(shaderDefinitionFile);
                material->SetProperties(properties);
            }

            if (properties.size() > 0) {
                if (ImGui::CollapsingHeader("Properties", true)) {
                    for (auto p : properties) {
                        //RPG::Log("Material Window", p->GetType());
                        std::any prop = p->GetProperty();
                        if (p->GetType() == "float") {
                            float v = std::any_cast<float>(prop);
                            if (ImGui::DragFloat(p->GetEditorName().c_str(), (float *) &v, 0.05f)) {
                                p->SetProperty(v);
                            }
                        } else if (p->GetType() == "glm::vec2") {
                            glm::vec2 v = std::any_cast<glm::vec2>(prop);
                            if (ImGui::DragFloat2(p->GetEditorName().c_str(), (float *) &v, 0.05f)) {
                                p->SetProperty(v);
                            }
                        } else if (p->GetType() == "glm::vec3") {
                            glm::vec3 v = std::any_cast<glm::vec3>(prop);
                            if (ImGui::DragFloat3(p->GetEditorName().c_str(), (float *) &v, 0.05f)) {
                                p->SetProperty(v);
                            }
                        } else if (p->GetType() == "glm::vec4") {
                            glm::vec4 v = std::any_cast<glm::vec4>(prop);
                            if (ImGui::ColorPicker4(p->GetEditorName().c_str(), (float *) &v, 0.05f)) {
                                p->SetProperty(v);
                            }
                        } else if (p->GetType() == "RPG::Resource::String") {
                            std::any prop = p->GetProperty();
                            std::string v = std::any_cast<std::string>(prop);
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75, 0.75, 0.75, 1));
                            std::string id = "##" + p->GetGuid();
                            ImGui::InputText(id.c_str(), &v, ImGuiInputTextFlags_ReadOnly);
                            ImGui::PopStyleColor();

                            if (ImGui::IsItemHovered()) {
                                ImGui::BeginTooltip();
                                ImGui::Text(v.c_str());
                                ImGui::EndTooltip();
                            }

                            //Dropable Payloads
                            if (p->AllowsDragAndDrop()) {
                                //Drop - payload
                                if (ImGui::BeginDragDropTarget()) {
                                    if (ImGui::AcceptDragDropPayload(p->DragAndDropTag().c_str())) {
                                        RPG::GeneralPayload generalPayload = RPG::EditorStats::GetInstance().GetPayload();
                                        RPG::Log("Payload", generalPayload.path);
                                        p->SetProperty(generalPayload.path);
                                    }
                                    ImGui::EndDragDropTarget();
                                }
                            }

                            ImGui::SameLine();
                            ImGui::Text(p->GetName().c_str(), p->GetEditorName().c_str());

                            if (p->DragAndDropTag() == "Texture") {
                                std::string texturePath = std::any_cast<std::string>(p->GetProperty());
                                uint32_t id = RPG::Content::GetInstance().GetTextureID(texturePath);
                                if (id != -1) {
                                    ImGui::Image((void *) (intptr_t) id, ImVec2{50,50}, ImVec2{0, 1}, ImVec2{1, 0}, ImVec4{1,1,1,1}, ImVec4(1,1,1,1));
                                }
                            }
                        }
                    }
                }
            }
            ImGui::Spacing();

            if (ImGui::Button("Save")) {
                material->SetProperties(properties);
                RPG::Serializer::GetInstance().SaveMaterial(material, std::any_cast<std::string>(property->GetProperty()));
            }
        }

        ImGui::End();
    }

    void GrabProperties(std::string data) {
        properties.clear();
        json j = json::parse(data);
        for (auto [key, value] : j["Properties"].items()) {
            auto property = RPG::Assets::LoadProperty(value);
            if (property != nullptr) {
                properties.push_back(property);
            }
        }
        RPG::Log("Material Maker", "Was able to load (" + std::to_string(properties.size()) + ") properties");
    }
};

MaterialMakerWindow::MaterialMakerWindow(std::shared_ptr<RPG::OpenGLAssetManager> assetManager) : internal(RPG::MakeInternalPointer<Internal>(assetManager)) {}

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

std::shared_ptr<RPG::Material> MaterialMakerWindow::GetCurrentMaterial() {
    return internal->material;
}