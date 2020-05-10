/*
 *  UrhoApp.cxx
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

#include <iostream>
#include <Source.hxx>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <components/Vehicle.hxx>
#include <range/v3/algorithm/find_if.hpp>
#include "app/UrhoApp.hxx"
#include "components/MovableCamera.hxx"
#include "components/Registry.hxx"
#include "BoardConf.hxx"
#include "Runtime.hxx"
#include "Toolchain.hxx"
#include "UrhoUtility.hxx"
#include "VehicleConf.hxx"

UrhoApp::UrhoApp(Urho3D::Context* context) : Urho3D::Application{context} {
    MovableCamera::RegisterObject(context);
    TorchMenu::RegisterObject(context);
    Vehicle::RegisterObject(context);
    context_->RegisterSubsystem(this);
}

void UrhoApp::Setup() {
    engineParameters_[Urho3D::EP_WINDOW_TITLE] = "Smart Car Emul";
    engineParameters_[Urho3D::EP_LOG_NAME] = GetSubsystem<Urho3D::FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_[Urho3D::EP_FULL_SCREEN] = false;
    engineParameters_[Urho3D::EP_HEADLESS] = false;
    engineParameters_[Urho3D::EP_SOUND] = false;

    if (!engineParameters_.Contains(Urho3D::EP_RESOURCE_PREFIX_PATHS))
        engineParameters_[Urho3D::EP_RESOURCE_PREFIX_PATHS] = ";share/smce/Resources;";
}

void UrhoApp::Start() {
    auto& input = *GetSubsystem<Urho3D::Input>();
    input.SetMouseVisible(true);
    input.SetMouseGrabbed(false);

    create_scene();
    create_viewport();
    subscribe_to_events();
}

void UrhoApp::Stop() {}

bool UrhoApp::loadIno(smce::SketchSource ino_path, std::filesystem::path config_path) {
    std::string loc = ino_path.location.generic_string();

    auto vehicle_conf_opt = smce::load(config_path);
    if (!vehicle_conf_opt)
        return false;
    b_data = smce::as_data(*vehicle_conf_opt);
    b_data.silence_errors = false;
    b_data.write_byte = +[](unsigned char c) { return static_cast<bool>(std::cout.put(c)); };
    b_data.write_buf = +[](const unsigned char* buf, std::size_t len) { return std::cout.write(reinterpret_cast<const char*>(buf), len) ? len : 0; };

    b_info = smce::as_info(*vehicle_conf_opt);

    auto compile_result = smce::compile_sketch(ino_path, ".");
    if (std::holds_alternative<std::runtime_error>(compile_result)) {
        std::cout << std::get<std::runtime_error>(compile_result).what() << std::endl;
        return false;
    }

    auto sketch = std::get<smce::SketchObject>(compile_result);

    ino_runtime.set_sketch_and_car(std::move(sketch), b_data, b_info);
    return true;
}

void UrhoApp::create_vehicle() {
    m_vehicle_node->SetPosition(Urho3D::Vector3(0.0f, 2.0f, 0.0f));
    m_vehicle = m_vehicle_node->CreateComponent<Vehicle>();
    if (m_vehicle)
        m_vehicle->Init();
}

void UrhoApp::create_scene() {
    auto* cache = GetSubsystem<Urho3D::ResourceCache>();
    cache->AddResourceDir("share/smce/Torch");
    m_scene = Urho3D::MakeShared<Urho3D::Scene>(context_);
    m_scene->CreateComponent<Urho3D::Octree>();
    auto* world = m_scene->CreateComponent<Urho3D::PhysicsWorld>();
    context_->RegisterSubsystem(world);

    Urho3D::Node* ui_node = m_scene->CreateChild("GUI");

    m_gui = ui_node->CreateComponent<TorchMenu>();

    Urho3D::Node* plane_node = m_scene->CreateChild("Ground");
    plane_node->SetScale(Urho3D::Vector3(200, 0, 200));
    auto* const planeObject = plane_node->CreateComponent<Urho3D::StaticModel>();
    planeObject->SetModel(cache->GetResource<Urho3D::Model>("Models/Cone.mdl"));
    auto* body = plane_node->CreateComponent<Urho3D::RigidBody>();
    body->SetCollisionLayer(2);
    auto* shape = plane_node->CreateComponent<Urho3D::CollisionShape>();
    shape->SetStaticPlane();

    Urho3D::Node* lightNode = m_scene->CreateChild("Sunlight");
    auto* const light = lightNode->CreateComponent<Urho3D::Light>();
    light->SetLightType(Urho3D::LIGHT_DIRECTIONAL);

    m_camera_node = m_scene->CreateChild("Camera");
    m_camera_node->CreateComponent<MovableCamera>();
    m_camera_node->SetPosition(Urho3D::Vector3(0.0f, 100.0f, 0.0f));

    m_vehicle_node = m_scene->CreateChild("Vehicle");
    m_vehicle_node->SetPosition(Urho3D::Vector3(0.0f, 1.0f, 0.0f));
}

void UrhoApp::create_viewport() {
    auto* renderer = GetSubsystem<Urho3D::Renderer>();
    auto viewport = Urho3D::MakeShared<Urho3D::Viewport>(context_, m_scene, m_camera_node->GetComponent<MovableCamera>());
    renderer->SetViewport(0, viewport);
}

void UrhoApp::setup_attachments(BoardData& board, const smce::VehicleConfig& vconf) {
    for (const auto& [type_str, jconf] : vconf.attachments) {
        const auto& type = type_str;
        auto* const it = ranges::find_if(attachments_registry, [&](const auto& pair) { return pair.first == type; });
        if (it == attachments_registry.end()) {
            Urho3D::Log::Write(Urho3D::LOG_WARNING, Urho3D::String("Unknown component type ") + type_str.c_str());
            continue;
        }

        auto* const att = it->second(board, m_vehicle_node.Get(), *jconf);
        m_vehicle_attachments.push_back(att);
        m_vehicle_node->AddComponent(att, 0, Urho3D::REPLICATED);
    }
}

void UrhoApp::subscribe_to_events() {
    SubscribeToEvent(Urho3D::E_UPDATE, URHO3D_HANDLER(UrhoApp, HandleUpdate));
    SubscribeToEvent(m_gui, E_INO_FOUND, URHO3D_HANDLER(UrhoApp, HandleIno));
}

void UrhoApp::HandleIno(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data) {
    std::string path = event_data[inoFileFound::P_INO_PATH].GetString().CString();
    cp = std::async([&, path]() { return loadIno({path}, "/home/ruthgerd/demoo/board_config.json"); });
    compile_check = true;
}

void UrhoApp::spawnSmartCar() {
    if (cp.wait_for(std::chrono::seconds(0)) == std::future_status::ready && m_vehicle.Null()) {
        auto vconf = smce::VehicleConfig::load("/home/ruthgerd/demoo/vehicle_config.json");
        if (vconf) {
            create_vehicle();
            setup_attachments(b_data, *vconf);
            runtime = std::async([&]() { return ino_runtime.start(); });
        } else {
            std::cout << "vconf bad" << std::endl;
        }
        Urho3D::VariantMap map;
        map[inoCompiled::P_INO_COMPILED] = true;
        SendEvent(E_INO_COMPILED, map);
        input_tr = std::thread([&]() {
            while (true) {
                std::string str;
                std::cout.flush();
                std::getline(std::cin, str);
                if (str == "q")
                    break;
                std::scoped_lock l{b_data.uart_buses[0].rx_mutex};
                const auto original_buf_len = b_data.uart_buses[0].rx.size();
                b_data.uart_buses[0].rx.resize(b_data.uart_buses[0].rx.size() + str.size());
                std::transform(str.begin(), str.end(), b_data.uart_buses[0].rx.begin() + original_buf_len,
                               [](char c) { return static_cast<std::byte>(c); });
            }
        });
        compile_check = false;
    }
}

void UrhoApp::HandleUpdate(Urho3D::StringHash, Urho3D::VariantMap& event_data) {
    const float delta_time = event_data[Urho3D::Update::P_TIMESTEP].GetFloat();
    m_camera_node->GetComponent<MovableCamera>()->move(delta_time);
    if (compile_check)
        spawnSmartCar();
}
