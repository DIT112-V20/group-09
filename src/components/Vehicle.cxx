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

//
// Copyright (c) 2008-2020 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

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
    URHO3D_ATTRIBUTE("Controls Yaw", float, controls_.yaw_, 0.0f, Urho3D::AM_DEFAULT);
    URHO3D_ATTRIBUTE("Controls Pitch", float, controls_.pitch_, 0.0f, Urho3D::AM_DEFAULT);
}

Vehicle::Vehicle(Urho3D::Context* context) : LogicComponent(context) {
    SetUpdateEventMask(Urho3D::USE_FIXEDUPDATE | Urho3D::USE_POSTUPDATE);
}

Vehicle::~Vehicle() = default;

void Vehicle::Init() {
    auto* vehicle = node_->CreateComponent<Urho3D::RaycastVehicle>();
    vehicle->Init();
    auto* hullBody = node_->GetComponent<Urho3D::RigidBody>();
    // the car weigth is 1.33kg
    hullBody->SetMass(1.33f);
    // LinearDamping & AngularDamping depend on Velocity, which i dont have at the moment
    hullBody->SetLinearDamping(0.1f);
    hullBody->SetAngularDamping(0.4f);
    hullBody->SetCollisionLayer(1);
    // This function is called only from the main program when initially creating the vehicle, not on scene load
    auto* cache = GetSubsystem<Urho3D::ResourceCache>();
    auto* hullObject = node_->CreateComponent<Urho3D::StaticModel>();
    // Setting-up collision shape
    auto* hullColShape = node_->CreateComponent<Urho3D::CollisionShape>();
    Urho3D::Vector3 v3BoxExtents = Urho3D::Vector3::ONE;
    hullColShape->SetBox(v3BoxExtents);
    node_->SetScale(Urho3D::Vector3(0.195f, 0.15f, 0.236f)); /// Chassi 195mm*15mm*236mm
    hullObject->SetModel(cache->GetResource<Urho3D::Model>("Models/SmartCar.mdl"));
    hullObject->SetMaterial(cache->GetResource<Urho3D::Material>("Materials/offroadVehicle.xml"));
    hullObject->SetCastShadows(true);
    float connectionHeight = -0.06f;
    bool isFrontWheel = true;
    Urho3D::Vector3 wheelDirection(0, -1, 0);
    Urho3D::Vector3 wheelAxle(-1, 0, 0);
   
    float wheelX = 0.09f;
    float wheelZ = 0.08f;
    // Front left
    connectionPoints_[0] = Urho3D::Vector3(-wheelX, connectionHeight, wheelZ);
    // Front right
    connectionPoints_[1] = Urho3D::Vector3(wheelX, connectionHeight, wheelZ);
    // Back left
    connectionPoints_[2] = Urho3D::Vector3(-wheelX, connectionHeight, -wheelZ);
    // Back right
    connectionPoints_[3] = Urho3D::Vector3(wheelX, connectionHeight, -wheelZ);
    const Urho3D::Color LtBrown(0.972f, 0.780f, 0.412f);
    for (int id = 0; id < connectionPoints_.size(); id++) {
        Urho3D::Node* wheelNode = GetScene()->CreateChild();
        Urho3D::Vector3 connectionPoint = connectionPoints_[id];
        
        bool isFrontWheel = connectionPoints_[id].z_ > 0.0f;
        wheelNode->SetRotation(connectionPoint.x_ >= 0.0 ? Urho3D::Quaternion(0.0f, 0.0f, -90.0f) : Urho3D::Quaternion(0.0f, 0.0f, 90.0f));
        wheelNode->SetWorldPosition(node_->GetWorldPosition() + node_->GetWorldRotation() * connectionPoints_[id]);
        vehicle->AddWheel(wheelNode, wheelDirection, wheelAxle, suspensionRestLength_, wheelRadius_, isFrontWheel);
        vehicle->SetWheelSuspensionStiffness(id, suspensionStiffness_);
        vehicle->SetWheelDampingRelaxation(id, suspensionDamping_);
        vehicle->SetWheelDampingCompression(id, suspensionCompression_);
        vehicle->SetWheelFrictionSlip(id, wheelFriction_);
        vehicle->SetWheelRollInfluence(id, rollInfluence_);
        wheelNode->SetScale(Urho3D::Vector3(0.067f, 0.023f, 0.067f));// tire Diameter: 67mm
        auto* pWheel = wheelNode->CreateComponent<Urho3D::StaticModel>();
        pWheel->SetModel(cache->GetResource<Urho3D::Model>("Models/SmartTire.mdl"));
        pWheel->SetMaterial(cache->GetResource<Urho3D::Material>("Materials/Stone.xml"));
        pWheel->SetCastShadows(true);
    }
    vehicle->ResetWheels();
}
void Vehicle::resetDifferential() {
    auto* vehicle = node_->GetComponent<Urho3D::RaycastVehicle>();
    engineForce_ = 0;
    vehicle->SetEngineForce(0, 0);
    vehicle->SetEngineForce(2, 0);
    vehicle->SetEngineForce(1, 0);
    vehicle->SetEngineForce(3, 0);
}
void Vehicle::setDifferential(float engineForce, const unsigned int control) {
    // the Control checks whether you want to turn left or right.
    // will remove this once i got the Differential.cxx to work
    auto* vehicle = node_->GetComponent<Urho3D::RaycastVehicle>();
    if (control == 8) {
        engineForce = engineForce * -2;
    } else {
        engineForce = engineForce * 2;
    }
    vehicle->SetEngineForce(1, engineForce);
    vehicle->SetEngineForce(3, engineForce);
    vehicle->SetEngineForce(0, -engineForce);
    vehicle->SetEngineForce(2, -engineForce);
}
void Vehicle::FixedUpdate(float timeStep) {
    float accelerator = 0.0f;
    auto* vehicle = node_->GetComponent<Urho3D::RaycastVehicle>();
    // Read controls

    if (controls_.buttons_ & CTRL_LEFT) {
        setDifferential(maxEngineForce_, CTRL_LEFT);
    }
    if (controls_.buttons_ & CTRL_RIGHT) {
        setDifferential(maxEngineForce_, CTRL_RIGHT);
    }
    if (controls_.buttons_ & CTRL_FORWARD) {
        resetDifferential();
        accelerator = 0.3f;     // not accurate
    }
    if (controls_.buttons_ & CTRL_BACK) {
        resetDifferential();
        accelerator = -0.3f;    // not accurate
    }

    // apply forces
    engineForce_ = maxEngineForce_ * accelerator;
   /* vehicle->SetEngineForce(0, engineForce_);
    vehicle->SetEngineForce(1, engineForce_);*/
    vehicle->SetEngineForce(3, engineForce_);
    vehicle->SetEngineForce(2, engineForce_);
}

void Vehicle::PostUpdate(float timeStep) {
    auto* vehicle = node_->GetComponent<Urho3D::RaycastVehicle>();
    auto* vehicleBody = node_->GetComponent<Urho3D::RigidBody>();
    Urho3D::Vector3 velocity = vehicleBody->GetLinearVelocity();
    Urho3D::Vector3 accel = (velocity - prevVelocity_) / timeStep;
    float planeAccel = Urho3D::Vector3(accel.x_, 0.0f, accel.z_).Length();
    prevVelocity_ = velocity;
}
