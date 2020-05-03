/*
 *  Registry.hxx
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

#ifndef SMARTCAR_EMUL_REGISTRY_HXX
#define SMARTCAR_EMUL_REGISTRY_HXX

#include <array>
#include <string_view>
#include <utility>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Scene/Node.h>
#include <rapidjson/document.h>
#include "components/PerfectAnalogLaserSensor.hxx"
#include "components/WheelServo.hxx"
#include "BoardData.hxx"

using namespace std::literals;
constexpr std::array<std::pair<std::string_view, Urho3D::LogicComponent* (*)(BoardData&, Urho3D::Node*, const rapidjson::Value&)>, 3>
    attachments_registry{
        std::pair{"ServoMotor"sv,
                  +[](BoardData& bd, Urho3D::Node* node, const rapidjson::Value& conf) -> Urho3D::LogicComponent* {
                      return new ServoMotor{bd, node, conf};
                  }},
        std::pair{"SteeringServo"sv,
                  +[](BoardData& bd, Urho3D::Node* node, const rapidjson::Value& conf) -> Urho3D::LogicComponent* {
                      return new SteeringServo{bd, node, conf};
                  }},
        std::pair{"PerfectAnalogLaserSensor"sv, +[](BoardData& bd, Urho3D::Node* node, const rapidjson::Value& conf) -> Urho3D::LogicComponent* {
                      return new PerfectAnalogLaserSensor{bd, node, conf};
                  }}};

#endif // SMARTCAR_EMUL_REGISTRY_HXX
