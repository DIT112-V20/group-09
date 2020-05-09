/*
 *  UrhoApp.hxx
 *  Copyright 2020 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef SMARTCAR_EMUL_URHOAPP_HXX
#define SMARTCAR_EMUL_URHOAPP_HXX

#include <vector>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <gui/TorchMenu.hxx>

struct BoardData;

namespace smce {
struct VehicleConfig;
}

class UrhoApp : public Urho3D::Application {
    URHO3D_OBJECT(UrhoApp, Urho3D::Application);

  public:
    explicit UrhoApp(Urho3D::Context* context);

    void Setup() override;
    void Start() override;
    void Stop() override;

  private:
    enum class Menu {
        none,
        main,
    };

    void create_scene();
    void create_viewport();
    void setup_attachments(BoardData& board, const smce::VehicleConfig&);
    void subscribe_to_events();
    void HandleUpdate(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);

    Urho3D::SharedPtr<Urho3D::Scene> m_scene;
    Urho3D::SharedPtr<Urho3D::Node> m_camera_node;
    Urho3D::SharedPtr<Urho3D::Node> m_vehicle_node;
    Urho3D::SharedPtr<TorchMenu> m_gui;

    std::vector<Urho3D::LogicComponent*> m_vehicle_attachments;
};

#endif // SMARTCAR_EMUL_URHOAPP_HXX
