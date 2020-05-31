/*
 *  PerfectDistanceAnalogSensor.cxx
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

#include <array>
#include <string_view>
#include "components/PerfectDistanceAnalogSensor.hxx"
#include <fmt/printf.h>
#include <Urho3D/Scene/Node.h>
using namespace std::literals;

constexpr static std::array sens_types = {"GP2D120"sv, "GP2Y0A02"sv, "GP2Y0A21"sv};
constexpr static std::array sens_trans = {
    +[](std::uint32_t val) -> std::uint16_t { return (2909 - 5 * val) / (val + 1) ; },
    +[](std::uint32_t val) -> std::uint16_t { return 0.12f * (141 * val + 78850) / val; },
    +[](std::uint32_t val) -> std::uint16_t { return (3 * val + 6799) / (val + 4); }
};

PerfectDistanceAnalogSensor::PerfectDistanceAnalogSensor(BoardData& bd, Urho3D::Node* node, const rapidjson::Value& conf) : LaserCaster{node} {
    constexpr auto err{"Attempted to create component PerfectDistanceAnalogSensor with an invalid configuration"};
    if (!conf.HasMember("pin") || !conf.HasMember("type") || conf["pin"].GetUint() >= bd.analog_pin_values.size())
        throw std::runtime_error{err};
    fmt::print("distance type: {}\n", conf["type"].GetString());
    const auto it = std::find(sens_types.begin(), sens_types.end(), conf["type"].GetString());
    if(it == sens_types.end())
        throw std::runtime_error{err};

    m_type = std::distance(sens_types.begin(), it);
    m_pin_ptr = &bd.analog_pin_values[conf["pin"].GetUint()];
}
void PerfectDistanceAnalogSensor::Update(float timeStep) { m_pin_ptr->store(sens_trans[m_type](measure()));
}