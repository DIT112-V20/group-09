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
#include <gsl/gsl>
#include <nameof.hpp>
#include <rapidjson/document.h>
#include "components/PerfectDistanceAnalogSensor.hxx"
#include "components/PerfectDistanceI2CSensor.hxx"
#include "components/PerfectGyroscopeI2CSensor.hxx"
#include "components/WheelServo.hxx"
#include "BoardData.hxx"
#include "utility.hxx"

using namespace std::literals;

template <class T> constexpr auto attachment_entry() {
    return std::pair{nameof::nameof_type<T>(),
                     +[](BoardData& bd, Urho3D::Node* node, const rapidjson::Value& conf) -> gsl::owner<Urho3D::LogicComponent*> {
                         return new T{bd, node, conf};
                     }};
}

template <class... Attachments> constexpr auto compile_registry() { return make_array(attachment_entry<Attachments>()...); }

constexpr std::array attachments_registry =
    compile_registry<ServoMotor, SteeringServo, ServoMotor, PerfectDistanceI2CSensor, PerfectGyroscopeI2CSensor, PerfectDistanceAnalogSensor>();

#endif // SMARTCAR_EMUL_REGISTRY_HXX
