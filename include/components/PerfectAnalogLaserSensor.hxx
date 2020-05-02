/*
 *  PerfectAnalogLaserSensor.hxx
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

#ifndef SMARTCAR_EMUL_PERFECTANALOGLASERSENSOR_HXX
#define SMARTCAR_EMUL_PERFECTANALOGLASERSENSOR_HXX

#include <cstdint>
#include <rapidjson/include/rapidjson/document.h>
#include "BoardData.hxx"
#include "LaserCaster.hxx"

class PerfectAnalogLaserSensor : public LaserCaster {
    URHO3D_OBJECT(PerfectAnalogLaserSensor, LaserCaster);

    std::atomic_uint8_t value;

    std::uint8_t bus_id{};
    std::uint8_t address{};
    I2cBus* bus;

  public:
    PerfectAnalogLaserSensor(Urho3D::Context* context, BoardData& bd, Urho3D::Node* node, const rapidjson::Value& pin);

    void Update(float timeStep) override;
};

#endif // SMARTCAR_EMUL_PERFECTANALOGLASERSENSOR_HXX
