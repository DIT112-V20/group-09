/*
 *  PerfectAnalogLaserSensor.cxx
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

#include "components/PerfectAnalogLaserSensor.hxx"

constexpr auto err_msg = "Attempted to create component PerfectAnalogLaserSensor with an invalid configuration";

PerfectAnalogLaserSensor::PerfectAnalogLaserSensor(Urho3D::Context* context, BoardData& bd, const rapidjson::Value& pin) : LaserCaster{context} {
    if (!pin.HasMember("pin"))
        throw std::runtime_error{err_msg};

    auto pin_no = pin["pin"].GetUint();

    if (pin_no > bd.analog_pin_values.size())
        throw std::runtime_error{err_msg};

    analog_pin = &bd.analog_pin_values[pin_no];
}

void PerfectAnalogLaserSensor::Update(float timeStep) { analog_pin->store(measure()); }
