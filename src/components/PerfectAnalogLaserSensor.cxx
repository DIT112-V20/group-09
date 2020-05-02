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

#include <range/v3/algorithm/find.hpp>
#include "components/PerfectAnalogLaserSensor.hxx"

constexpr auto err_msg = "Attempted to create component PerfectAnalogLaserSensor with an invalid configuration";

PerfectAnalogLaserSensor::PerfectAnalogLaserSensor(Urho3D::Context* context, BoardData& bd, Urho3D::Node* node, const rapidjson::Value& pin)
    : LaserCaster{context, node} {
    if (!pin.HasMember("bus_id") || pin.HasMember("address"))
        throw std::runtime_error{err_msg};

    bus_id = pin["bus_id"].GetUint();
    address = pin["address"].GetUint();

    bus = &bd.i2c_buses[bus_id];
    std::scoped_lock lock{bus->devices_mut};
    auto& [device_buf, device] = bus->slaves[address];
    device = [](size_t t) {

    };
}

void PerfectAnalogLaserSensor::Update(float timeStep) {}
