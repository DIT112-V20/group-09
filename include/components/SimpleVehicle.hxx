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

#ifndef SMARTCAR_EMUL_SIMPLEVEHICLE_HXX
#define SMARTCAR_EMUL_SIMPLEVEHICLE_HXX

#include <Urho3D/Core/Context.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Physics/PhysicsUtils.h>
#include <Urho3D/Scene/LogicComponent.h>
#include "VehicleConf.hxx"

class SimpleVehicle : public Urho3D::LogicComponent {
    URHO3D_OBJECT(SimpleVehicle, LogicComponent)

    float mass = 50;
    float wheel_y = -0.4;
    float wheel_radius = 0.125f;
    float wheel_x = 1.f;
    float wheel_z = wheel_radius * 8.f;
    float suspension_rest_length = 0.0345f;
    float suspension_stiffness = 100.61f;
    float suspension_damping = 8.0f;
    float suspension_compression = 13.1f;
    float wheel_friction = 20.f;
    float roll_influence = 0.02f;

    Urho3D::Node* hull_node;

  public:
    static void RegisterObject(Urho3D::Context* context) { context->RegisterFactory<SimpleVehicle>(); }
    SimpleVehicle(Urho3D::Context* context) : LogicComponent(context) {}

    void Init();
    void PostUpdate(float time_step) override;
};
#endif // SMARTCAR_EMUL_SIMPLEVEHICLE_HXX
