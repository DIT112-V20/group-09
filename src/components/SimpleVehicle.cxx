/*
 *  Vehicle.cxx
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

#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Graphics/DecalSet.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/Constraint.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RaycastVehicle.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include "components/SimpleVehicle.hxx"

void SimpleVehicle::Init() {
    auto* cache = GetSubsystem<Urho3D::ResourceCache>();

    node_->SetScale({0.575f, 0.25f, 0.75f});

    auto* vehicle = node_->CreateComponent<Urho3D::RaycastVehicle>();
    vehicle->Init();

    auto* hull_body = node_->GetComponent<Urho3D::RigidBody>();
    hull_body->SetMass(mass);
    hull_body->SetCollisionLayer(1);

    node_->CreateComponent<Urho3D::CollisionShape>()->SetBox(Urho3D::Vector3::ONE);

    auto* hullObject = node_->CreateComponent<Urho3D::StaticModel>();
    hullObject->SetModel(cache->GetResource<Urho3D::Model>("Torch/Data/Models/SmartCar.mdl"));
    hullObject->SetMaterial(cache->GetResource<Urho3D::Material>("Torch/Data/Textures/solid_red.xml"));
    hullObject->SetCastShadows(true);

    Urho3D::Vector3 wheelDirection(0, -1, 0);
    Urho3D::Vector3 wheelAxle(-1, 0, 0);

    std::vector<Urho3D::Vector3> connectionPoints_{
        {-wheel_x, wheel_y, wheel_z}, {wheel_x, wheel_y, wheel_z}, {-wheel_x, wheel_y, -wheel_z}, {wheel_x, wheel_y, -wheel_z}};

    for (size_t id = 0; id < connectionPoints_.size(); ++id) {
        auto* wheelNode = GetScene()->CreateChild();
        auto connectionPoint = connectionPoints_[id];
        bool isFrontWheel = connectionPoints_[id].z_ > 0.0f;
        wheelNode->SetRotation(connectionPoint.x_ >= 0.0 ? Urho3D::Quaternion(0.0f, 0.0f, -90.0f) : Urho3D::Quaternion(0.0f, 0.0f, 90.0f));
        wheelNode->SetWorldPosition((node_->GetWorldPosition() + node_->GetWorldRotation() * connectionPoints_[id]) / 4);
        wheelNode->SetScale({0.25f, 0.16f, 0.25f});

        auto* pWheel = wheelNode->CreateComponent<Urho3D::StaticModel>();
        pWheel->SetModel(cache->GetResource<Urho3D::Model>("Models/Cylinder.mdl"));
        pWheel->SetMaterial(cache->GetResource<Urho3D::Material>("Torch/Data/Textures/solid_black.xml"));
        pWheel->SetCastShadows(true);

        vehicle->AddWheel(wheelNode, wheelDirection, wheelAxle, suspension_rest_length, wheel_radius, isFrontWheel);
        vehicle->SetWheelSuspensionStiffness(id, suspension_stiffness);
        vehicle->SetWheelDampingRelaxation(id, suspension_damping);
        vehicle->SetWheelDampingCompression(id, suspension_compression);
        vehicle->SetWheelFrictionSlip(id, wheel_friction);
        vehicle->SetWheelRollInfluence(id, roll_influence);
    }
    vehicle->ResetWheels();
}

void SimpleVehicle::PostUpdate(float time_step) {
    auto* vehicle = node_->GetComponent<Urho3D::RaycastVehicle>();
    if (vehicle)
        for (size_t i = 0; i < vehicle->GetNumWheels(); ++i) {
            if (vehicle->GetEngineForce(i) == 0)
                vehicle->SetBrake(i, 1);
            else
                vehicle->SetBrake(i, 0);
        }
}
