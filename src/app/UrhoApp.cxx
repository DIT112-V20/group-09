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
#include <Urho3D/Physics/RaycastVehicle.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/UI.h>
#include <range/v3/algorithm/find_if.hpp>
#include "app/UrhoApp.hxx"
#include "components/MovableCamera.hxx"
#include "components/Registry.hxx"
#include "components/WheelServo.hxx"
#include "UrhoUtility.hxx"
#include "Vehicle.hxx"
#include "VehicleConf.hxx"

UrhoApp::UrhoApp(Urho3D::Context* context) : Urho3D::Application{context} {
    MovableCamera::RegisterObject(context);
    Vehicle::RegisterObject(context);
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

    cp = std::async([&]() { return loadIno({"/home/ruthgerd/demoo/test.ino"}, "/home/ruthgerd/demoo/board_config.json"); });
    create_scene();
    create_viewport();
    subscribe_to_events();
}

void UrhoApp::Stop() {}

bool UrhoApp::loadIno(smce::SketchSource ino_path, std::filesystem::path config_path) {
    auto vehicle_conf = smce::load(config_path);
    if (!vehicle_conf)
        return false;
    b_data = smce::as_data(*vehicle_conf);
    b_data.silence_errors = false;
    b_data.write_byte = +[](unsigned char c) { return static_cast<bool>(std::cout.put(c)); };
    b_data.write_buf = +[](const unsigned char* buf, std::size_t len) { return std::cout.write(reinterpret_cast<const char*>(buf), len) ? len : 0; };

    b_info = smce::as_info(*vehicle_conf);

    auto compile_result = smce::compile_sketch(ino_path, ".");
    if (std::holds_alternative<std::runtime_error>(compile_result)) {
        std::cout << std::get<std::runtime_error>(compile_result).what() << std::endl;
        return false;
    }

    auto sketch = std::get<smce::SketchObject>(compile_result);

    ino_runtime.set_sketch_and_car(std::move(sketch), b_data, b_info);

    return true;
}

void UrhoApp::create_scene() {
    auto* const cache = GetSubsystem<Urho3D::ResourceCache>();
    m_scene = Urho3D::MakeShared<Urho3D::Scene>(context_);
    m_scene->CreateComponent<Urho3D::Octree>();
    auto* world = m_scene->CreateComponent<Urho3D::PhysicsWorld>();
    context_->RegisterSubsystem(world);

    Urho3D::Node* plane_node = m_scene->CreateChild("Ground");
    plane_node->SetScale(Urho3D::Vector3(200, 0, 200));
    auto* const planeObject = plane_node->CreateComponent<Urho3D::StaticModel>();
    planeObject->SetModel(cache->GetResource<Urho3D::Model>("Models/Cone.mdl"));
    planeObject->SetMaterial(cache->GetResource<Urho3D::Material>("Materials/StoneTiled.xml"));
    auto* body = plane_node->CreateComponent<Urho3D::RigidBody>();
    body->SetCollisionLayer(2);
    auto* shape = plane_node->CreateComponent<Urho3D::CollisionShape>();
    shape->SetStaticPlane();

    top_light = m_scene->CreateChild("Light2");
    top_light->Pitch(90);
    auto* light = top_light->CreateComponent<Urho3D::Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetCastShadows(true);

    text_ = new Urho3D::Text(context_);
    text_->SetText("Experimental - SimpleCar + AckermanControl");
    text_->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 20);
    text_->SetColor(Color(1, 0, 0.1));
    text_->SetHorizontalAlignment(HA_CENTER);
    text_->SetVerticalAlignment(VA_TOP);
    text_->SetFontSize(25);
    GetSubsystem<UI>()->GetRoot()->AddChild(text_);

    m_camera_node = m_scene->CreateChild("Camera");
    m_camera_node->CreateComponent<MovableCamera>();
    m_camera_node->SetPosition(Urho3D::Vector3(0.0f, 30.0f, 0.0f));

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
        auto* t = m_vehicle_node->GetComponent<Urho3D::RaycastVehicle>();
        auto count = t->GetNumWheels();
        const auto& type = type_str;
        const auto it = ranges::find_if(attachments_registry, [&](const auto& pair) { return pair.first == type; });
        if (it == attachments_registry.end()) {
            Urho3D::Log::Write(Urho3D::LOG_WARNING, Urho3D::String("Unkown component type ") + type_str.c_str());
            continue;
        }

        auto* const att = it->second(board, m_vehicle_node.Get(), *jconf);
        m_vehicle_attachments.push_back(att);
        m_vehicle_node->AddComponent(att, 0, Urho3D::REPLICATED);
    }
}

void UrhoApp::subscribe_to_events() {
    SubscribeToEvent(Urho3D::E_KEYUP, URHO3D_HANDLER(UrhoApp, HandleKeyUp));
    SubscribeToEvent(Urho3D::E_UPDATE, URHO3D_HANDLER(UrhoApp, HandleUpdate));
    SubscribeToEvent(Urho3D::E_MOUSEBUTTONDOWN, URHO3D_HANDLER(UrhoApp, HandleMouseButtonDown));
}

void UrhoApp::HandleKeyUp(Urho3D::StringHash, Urho3D::VariantMap& event_data) {
    const auto key = event_data[Urho3D::KeyUp::P_KEY].GetInt();

    switch (key) {
    case Urho3D::KEY_F12:
        m_console->SetVisible(!m_console->IsVisible());
        break;
    case Urho3D::KEY_ESCAPE:
        engine_->Exit();
        break;
    default:
        break;
    }
}

void UrhoApp::HandleUpdate(Urho3D::StringHash, Urho3D::VariantMap& event_data) {
    const float delta_time = event_data[Urho3D::Update::P_TIMESTEP].GetFloat();
    m_camera_node->GetComponent<MovableCamera>()->move(delta_time);
    if (cp.wait_for(std::chrono::seconds(0)) == std::future_status::ready && m_vehicle.Null()) {
        auto vconf = smce::VehicleConfig::load("/home/ruthgerd/demoo/vehicle_config.json");
        if (vconf) {
            create_vehicle();
            setup_attachments(b_data, *vconf);
            runtime = std::async([&]() { return ino_runtime.start(); });
        } else {
            std::cout << "vconf bad" << std::endl;
        }
    }
}

void UrhoApp::HandleMouseButtonDown(Urho3D::StringHash, Urho3D::VariantMap&) {
    auto* const input = GetSubsystem<Urho3D::Input>();
    input->SetMouseMode(input->GetMouseMode() == Urho3D::MM_ABSOLUTE ? Urho3D::MM_RELATIVE : Urho3D::MM_ABSOLUTE);
}

void UrhoApp::create_vehicle() {
    Node* vehicleNode = m_vehicle_node;
    vehicleNode->SetPosition(Vector3(0.0f, 2.0f, 0.0f));
    m_vehicle = vehicleNode->CreateComponent<Vehicle>();
    m_vehicle->Init();
}