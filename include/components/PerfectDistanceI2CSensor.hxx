/*
 *  PerfectDistanceI2CSensor.hxx
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

#ifndef SMARTCAR_EMUL_PERFECTDISTANCEI2CSENSOR_HXX
#define SMARTCAR_EMUL_PERFECTDISTANCEI2CSENSOR_HXX

#include <cstdint>
#include <rapidjson/include/rapidjson/document.h>
#include "BoardData.hxx"
#include "DeviceMap.hxx"
#include "LaserCaster.hxx"

class PerfectDistanceI2CSensor : public LaserCaster {
  URHO3D_OBJECT(PerfectDistanceI2CSensor, LaserCaster);

    std::uint8_t bus_id{};
    I2cBus* bus;

    enum ReadRegs {
        INTERRUPT_RESULT = 0x13,
        MEASUREMENT_RESULT = 0x14,
    };

    enum WriteRegs { SPAD_INFO = 0x83, DEVICE_ID = 0xC0, CHANGE_ADDRESS = 0x8A };

    using RDev = regmon::Device<PerfectDistanceI2CSensor>;
    const RDev::DeviceMap vlx = RDev::make_device(
        RDev::DefaultsTo<DEVICE_ID>{0xEE}, RDev::DefaultsTo<SPAD_INFO>{0x01}, RDev::DefaultsTo<INTERRUPT_RESULT>{0x07},
        RDev::InvokesFunction<CHANGE_ADDRESS>{
                                     +[](PerfectDistanceI2CSensor& drv, RDev::DeviceStorage& store, gsl::span<const std::byte> incoming) {
                                       auto ex = drv.bus->slaves.extract(store.address);
                                       store.data[CHANGE_ADDRESS] = store.address = ex.key() = static_cast<uint8_t>(incoming.front());
                                       drv.bus->slaves.insert(std::move(ex));
                                     }});
    RDev::DeviceStorage store{};

  public:
    PerfectDistanceI2CSensor(BoardData& bd, Urho3D::Node* node, const rapidjson::Value& pin);

    void Update(float timeStep) override;
};

#endif // SMARTCAR_EMUL_PERFECTDISTANCEI2CSENSOR_HXX
