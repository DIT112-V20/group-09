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

#include <thread>
#include <vector>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <VehicleConf.hxx>
#include "BoardConf.hxx"
#include "Runtime.hxx"
#include "Toolchain.hxx"
#include "Vehicle.hxx"

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
    void create_vehicle();
    void create_scene();
    void create_viewport();
    void setup_attachments(BoardData& board, const smce::VehicleConfig&);
    void subscribe_to_events();
    void HandleUpdate(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void HandleKeyUp(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void HandleMouseButtonDown(Urho3D::StringHash, Urho3D::VariantMap&);

    Urho3D::SharedPtr<Urho3D::Scene> m_scene;
    Urho3D::SharedPtr<Urho3D::Node> m_camera_node;
    Urho3D::SharedPtr<Urho3D::Node> m_vehicle_node;
    Urho3D::SharedPtr<Urho3D::Console> m_console;
    std::vector<Urho3D::LogicComponent*> m_vehicle_attachments;

    Urho3D::SharedPtr<Vehicle> m_vehicle;
    bool loadIno(smce::SketchSource ino_path, std::filesystem::path config_path);
    BoardData b_data;
    BoardInfo b_info;
    smce::VehicleConfig vehicle_conf;
    smce::SketchRuntime ino_runtime{};

    std::future<bool> cp;
    std::future<bool> runtime;
    SharedPtr<Urho3D::Text> text_;
    SharedPtr<Node> boxNode_;
    SharedPtr<Node> top_light;
};

#endif // SMARTCAR_EMUL_URHOAPP_HXX
