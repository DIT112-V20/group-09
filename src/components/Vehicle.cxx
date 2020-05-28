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
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/Constraint.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RaycastVehicle.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include "components/Vehicle.hxx"


void Vehicle::RegisterObject(Urho3D::Context* context) {
    context->RegisterFactory<Vehicle>();
}

Vehicle::Vehicle(Urho3D::Context* context) : LogicComponent(context) {
}
Vehicle::~Vehicle() = default;

void Vehicle::Init(const smce::VehicleConfig& vconf) {
    auto* vehicle = node_->CreateComponent<Urho3D::RaycastVehicle>();
    vehicle->Init();
    auto* hullBody = node_->GetComponent<Urho3D::RigidBody>();
    hullBody->SetMass(1.33f);
    hullBody->SetLinearDamping(0.2f);
    hullBody->SetAngularDamping(0.5f);
    hullBody->SetCollisionLayer(1);
    auto* cache = GetSubsystem<Urho3D::ResourceCache>();
    auto* hullObject = node_->CreateComponent<Urho3D::StaticModel>();
    auto* hullColShape = node_->CreateComponent<Urho3D::CollisionShape>();
    Urho3D::Vector3 v3BoxExtents = Urho3D::Vector3::ONE;
    hullColShape->SetBox(v3BoxExtents);
    node_->SetScale(Urho3D::Vector3(0.195f, 0.0666f, 0.236f)); /// Chassi 195mm*66.6mm*236mm
    hullObject->SetModel(cache->GetResource<Urho3D::Model>(vconf.hull_model_file.generic_string().c_str()));
    hullObject->SetMaterial(cache->GetResource<Urho3D::Material>("Materials/offroadVehicle.xml"));
    hullObject->SetCastShadows(true);
    Urho3D::Vector3 wheelDirection(0, -1, 0);
    Urho3D::Vector3 wheelAxle(-1, 0, 0);
    for (auto& i = vconf.parts.begin(); i != vconf.parts.end(); i++) {
        Urho3D::Node* partNode = GetNode()->CreateChild();
        partNode->SetPosition(i->second.position);
        partNode->SetRotation(Urho3D::Quaternion(i->second.rotation.Data()));
        // if (i->second.model_position_offset) {
        //         Urho3D::Node* partNode = GetNode()->CreateChild();
        //  }
        vehicle->AddWheel(partNode, wheelDirection, wheelAxle, suspensionRestLength_, wheelRadius_, true);
        int id = 0;
        vehicle->SetWheelSuspensionStiffness(id, suspensionStiffness_);
        vehicle->SetWheelDampingRelaxation(id, suspensionDamping_);
        vehicle->SetWheelDampingCompression(id, suspensionCompression_);
        vehicle->SetWheelFrictionSlip(id, wheelFriction_);
        vehicle->SetWheelRollInfluence(id, rollInfluence_);
        partNode->SetScale(Urho3D::Vector3(0.067f, 0.023f, 0.067f)); // tire Diameter: 67mm
        auto* pWheel = partNode->CreateComponent<Urho3D::StaticModel>();
        pWheel->SetModel(cache->GetResource<Urho3D::Model>(i->second.model_file.generic_string().c_str()));
        pWheel->SetMaterial(cache->GetResource<Urho3D::Material>("Materials/Stone.xml"));
        pWheel->SetCastShadows(true);
        id++;
    }
    vehicle->ResetWheels();
}

void Vehicle::PostUpdate(float timeStep) {
    auto* vehicle = node_->GetComponent<Urho3D::RaycastVehicle>();
    auto* vehicleBody = node_->GetComponent<Urho3D::RigidBody>();
    Urho3D::Vector3 velocity = vehicleBody->GetLinearVelocity();
    Urho3D::Vector3 accel = (velocity - prevVelocity_) / timeStep;
    float planeAccel = Urho3D::Vector3(accel.x_, 0.0f, accel.z_).Length();
    prevVelocity_ = velocity;
}
