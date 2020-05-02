/*
 *  ServoMotor.hxx
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

#ifndef SMARTCAR_EMUL_WHEELSERVO_HXX
#define SMARTCAR_EMUL_WHEELSERVO_HXX

#include <Urho3D/Physics/RaycastVehicle.h>
#include "Servo.hxx"

template <void (Urho3D::RaycastVehicle::*mptr)(int, float)> class WheelServo : public Servo {
    URHO3D_OBJECT(WheelServo, Servo);
    static constexpr auto err_msg = "Attempted to create component ServoMotor with an invalid configuration";

    Urho3D::RaycastVehicle* vehicle;
    std::vector<unsigned int> wheels;
    float max_speed;

  public:
    explicit WheelServo(Urho3D::Context* context, BoardData& bd, Urho3D::Node* node, const rapidjson::Value& pin)
        : Servo{context, bd, node, pin}, vehicle{node->GetComponent<Urho3D::RaycastVehicle>()}, wheels{} {
        if (!pin.HasMember("wheels"))
            throw std::runtime_error{err_msg};
        if (!pin.HasMember("max_speed"))
            throw std::runtime_error{err_msg};
        max_speed = pin["max_speed"].GetFloat();
        for (const auto& x : pin["wheels"].GetArray()) {
            auto val = x.GetUint();
            if (val < vehicle->GetNumWheels())
                throw std::runtime_error{err_msg};
            wheels.push_back(val);
        }
    }

    void Update(float timeStep) override {
        if constexpr (mptr == &Urho3D::RaycastVehicle::SetSteeringValue) {
            for (const auto& no : wheels)
                (vehicle->*mptr)(no, deg2rad(math_map(+pwm_pin->load(), 0, 180, movement_mode->angle_min, movement_mode->angle_max)));
        } else if constexpr (mptr == &Urho3D::RaycastVehicle::SetEngineForce) {
            for (const auto& no : wheels)
                (vehicle->*mptr)(no, math_map(static_cast<float>(+pwm_pin->load()), 0.0f, 180.0f, 0.0f, 1.0f) * max_speed * (dir_pin ? 1 : -1));
        }
    }
};

extern template class WheelServo<&Urho3D::RaycastVehicle::SetSteeringValue>;
extern template class WheelServo<&Urho3D::RaycastVehicle::SetEngineForce>;

using SteeringServo = WheelServo<&Urho3D::RaycastVehicle::SetSteeringValue>;
using ServoMotor = WheelServo<&Urho3D::RaycastVehicle::SetEngineForce>;

#endif // SMARTCAR_EMUL_WHEELSERVO_HXX
