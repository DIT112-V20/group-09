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

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Console.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include "app/UrhoApp.hxx"
#include "components/EmulGlue.hxx"
#include "components/MovableCamera.hxx"
#include "components/WheelServo.hxx"
#include "components/Registry.hxx"
#include "components/Vehicle.hxx"
#include "gui/TorchMenu.hxx"
#include "UrhoUtility.hxx"
#include "VehicleConf.hxx"

UrhoApp::UrhoApp(Urho3D::Context* context) : Urho3D::Application{context} {
    MovableCamera::RegisterObject(context);
    TorchMenu::RegisterObject(context);
    EmulGlue::RegisterObject(context);
    SteeringServo::RegisterObject(context);
    ServoMotor::RegisterObject(context);
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

    create_scene();
    create_viewport();
    subscribe_to_events();
}

void UrhoApp::Stop() {}

void UrhoApp::create_scene() {
    auto* const cache = GetSubsystem<Urho3D::ResourceCache>();
    m_scene = Urho3D::MakeShared<Urho3D::Scene>(context_);
    m_scene->CreateComponent<Urho3D::Octree>();
    m_scene->CreateComponent<EmulGlue>();
    m_scene->CreateComponent<TorchMenu>();
    auto* world = m_scene->CreateComponent<Urho3D::PhysicsWorld>();
    context_->RegisterSubsystem(world);

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

}

void UrhoApp::create_viewport() {
    auto* renderer = GetSubsystem<Urho3D::Renderer>();
    auto viewport = Urho3D::MakeShared<Urho3D::Viewport>(context_, m_scene, m_camera_node->GetComponent<MovableCamera>());
    renderer->SetViewport(0, viewport);
}

void UrhoApp::subscribe_to_events() { SubscribeToEvent(Urho3D::E_UPDATE, URHO3D_HANDLER(UrhoApp, HandleUpdate)); }

void UrhoApp::HandleUpdate(Urho3D::StringHash, Urho3D::VariantMap& event_data) {
    const float delta_time = event_data[Urho3D::Update::P_TIMESTEP].GetFloat();
}
