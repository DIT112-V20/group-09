/*
 *  Vehicle.hxx
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

#ifndef SMARTCAR_EMUL_VEHICLE_HXX
#define SMARTCAR_EMUL_VEHICLE_HXX

#include <Urho3D/Input/Controls.h>
#include <Urho3D/Physics/PhysicsUtils.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <array>
#include "VehicleConf.hxx"

class Vehicle : public Urho3D::LogicComponent {
    URHO3D_OBJECT(Vehicle, LogicComponent)

  public:
    explicit Vehicle(Urho3D::Context* context);
    ~Vehicle() override;
    static void RegisterObject(Urho3D::Context* context);
    void Init(const smce::VehicleConfig& vconf);
    void PostUpdate(float timeStep);


  private:
    float wheelRadius_ = 0.0335f;
    float suspensionRestLength_ = 0.0345f;
    float suspensionStiffness_ = 100.61f;
    float suspensionDamping_ = 8.0f;
    float suspensionCompression_ = 13.1f;
    // the total Friction of wheels equals 10.4272N assuming Friction coefficent is 0.8
    float wheelFriction_ = 2.7f;
    float rollInfluence_ = 0.01f;
    Urho3D::Vector3 prevVelocity_;
};
#endif // SMARTCAR_EMUL_VEHICLE_HXX