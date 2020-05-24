/*
 *  PerfectDistanceAnalogSensor.hxx
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

#ifndef SMARTCAR_EMUL_PERFECTDISTANCEANALOGSENSOR_HXX
#define SMARTCAR_EMUL_PERFECTDISTANCEANALOGSENSOR_HXX

#include <atomic>
#include <rapidjson/document.h>
#include "BoardData.hxx"
#include "LaserCaster.hxx"

class PerfectDistanceAnalogSensor : public LaserCaster {
    std::atomic_uint16_t* m_pin_ptr;
    std::uint8_t m_type;

  public:
    PerfectDistanceAnalogSensor(BoardData& bd, Urho3D::Node* node, const rapidjson::Value& conf);
    void Update(float timeStep) override;
};

#endif // SMARTCAR_EMUL_PERFECTDISTANCEANALOGSENSOR_HXX
