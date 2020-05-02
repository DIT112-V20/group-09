/*
 *  Servo.cxx
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

#include "components/Servo.hxx"

constexpr auto err_msg = "Attempted to create component Servo with an invalid configuration";

Servo::Servo(Urho3D::Context* context, BoardData& bd,Urho3D::Node* node, const rapidjson::Value& pin) : Urho3D::LogicComponent{context}, node{node} {
    if (!pin.HasMember("pin"))
        throw std::runtime_error{err_msg};

    auto pin_no = pin["pin"].GetArray();

    for (const auto& x : pin_no) {
        if (x.GetUint() > bd.analog_pin_values.size())
            throw std::runtime_error{err_msg};
    }

    dir_pin = &bd.digital_pin_values[pin_no[0].GetUint()];
    pwm_pin = &bd.pwm_values[pin_no[2].GetUint()];

    if (pin.HasMember("rotation")) {
        auto tmp = pin["rotation"].GetObject();
        auto& x = movement_mode.emplace() = {tmp["angle_min"].GetFloat(), tmp["angle_max"].GetFloat()};
    }
}

void Servo::Update(float timeStep) {
    auto rotate = node_->GetRotation();
    auto val = +pwm_pin->load();
    Urho3D::Vector3 rotation{};
    if (movement_mode)
        rotation.y_ = deg2rad(math_map(val, 0, 180, movement_mode->angle_min, movement_mode->angle_max));
    else {
        rotation = rotate.EulerAngles();
        rotation.y_ += deg2rad(static_cast<decltype(rotation.y_)>(val));
    }
    node_->SetRotation({rotation.x_, rotation.y_, rotation.z_});
}
