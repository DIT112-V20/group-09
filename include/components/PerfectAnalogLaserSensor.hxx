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
#include "DeviceMap.hxx"
#include "BoardData.hxx"
#include "LaserCaster.hxx"

class PerfectAnalogLaserSensor : public LaserCaster {
    URHO3D_OBJECT(PerfectAnalogLaserSensor, LaserCaster);

    static constexpr auto ADDRESS_DEFAULT = 0x29;

    std::uint8_t bus_id{};
    I2cBus* bus;

    enum readRegs {
        MEASUREMENT_RESULT = 0x14,
    };

    enum writeRegs { SPAD_INFO = 0x83, DEVICE_ID = 0xC0, CHANGE_ADDRESS = 0x8A };

    using p_laser_map = regmon::Device<PerfectAnalogLaserSensor>;
    p_laser_map::DeviceMap vlx =
        p_laser_map::make_device(p_laser_map::DefaultsTo<DEVICE_ID>{0xEE}, p_laser_map::DefaultsTo<SPAD_INFO>{0x01},
                                 p_laser_map::InvokesFunction<CHANGE_ADDRESS>{
                                     +[](PerfectAnalogLaserSensor& drv, p_laser_map::DeviceStorage& store, gsl::span<const std::byte> incoming) {
                                         auto ex = drv.bus->slaves.extract(store.address);
                                         store.data[CHANGE_ADDRESS] = store.address = ex.key() = static_cast<uint8_t>(incoming.front());
                                         drv.bus->slaves.insert(std::move(ex));
                                     }});
    p_laser_map::DeviceStorage store = p_laser_map::DeviceStorage{};

  public:
    PerfectAnalogLaserSensor(Urho3D::Context* context, BoardData& bd, Urho3D::Node* node, const rapidjson::Value& pin);

    void Update(float timeStep) override;
};

#endif // SMARTCAR_EMUL_PERFECTANALOGLASERSENSOR_HXX
