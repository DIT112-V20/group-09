/*
 *  PerfectGyroscopeI2CSensor.hxx
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

#ifndef SMARTCAR_EMUL_PERFECTGYROSCOPEI2CSENSOR_HXX
#define SMARTCAR_EMUL_PERFECTGYROSCOPEI2CSENSOR_HXX

#include <array>
#include <bit>
#include <rapidjson/include/rapidjson/document.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Core/Context.h>
#include "BoardData.hxx"
#include "DeviceMap.hxx"


class PerfectGyroscopeI2CSensor : public Urho3D::LogicComponent {
  URHO3D_OBJECT(PerfectGyroscopeI2CSensor, Urho3D::LogicComponent);

    std::function<void()> ticker;

    enum Registers {
        WHO_AM_I = 0xF0,
        CTRL_REG1 [[maybe_unused]] = 0x20,
        CTRL_REG2 [[maybe_unused]] = 0x21,
        CTRL_REG3 [[maybe_unused]] = 0x22,
        CTRL_REG4 [[maybe_unused]] = 0x23,
        CTRL_REG5 [[maybe_unused]] = 0x24,
        REFERENCE [[maybe_unused]] = 0x25,
        OUT_TEMP = 0x26,
        STATUS_REG [[maybe_unused]] = 0x27,
        OUT_X_L = 0x28,
        OUT_X_H = 0x29,
        OUT_Y_L = 0x2A,
        OUT_Y_H = 0x2B,
        OUT_Z_L = 0x2C,
        OUT_Z_H = 0x2D,
    };

    enum class PowerMode {
        powered_down,
        sleeping,
        normal,
    };
    enum class DataRate {
        hz100 [[maybe_unused]],
        hz200 [[maybe_unused]],
        hz400 [[maybe_unused]],
        hz800 [[maybe_unused]],
    };
    enum class FullScale {
        dps250,
        dps500,
        dps2000,
    };
    enum Enabled {
        x = 1,
        y = 2,
        z = 4,
    };

    // Full scale to sensitivity
    constexpr static std::array fs2sens{0.00875f, 0.0175f, 0.07f};

    using RDev = regmon::Device<PerfectGyroscopeI2CSensor>;
    const RDev::DeviceMap gy = RDev::make_device(
        RDev::DefaultsTo<WHO_AM_I>{0b11010011},
        RDev::DefaultsTo<OUT_TEMP>{20} // °C
    );

    RDev::DeviceStorage store{};
    I2cBus* bus;

    [[maybe_unused]] [[nodiscard]] bool is_powered_up() const noexcept;
    [[nodiscard]] PowerMode get_power_mode() const noexcept;
    [[maybe_unused]] [[nodiscard]] DataRate get_data_rate() const noexcept;
    [[nodiscard]] FullScale get_full_scale() const noexcept;
    [[nodiscard]] std::endian get_data_endianness() const noexcept;
  public:
    PerfectGyroscopeI2CSensor(BoardData& bd, Urho3D::Node* node, const rapidjson::Value& pin);
    void UpdatePy(Urho3D::StringHash, Urho3D::VariantMap& event_data);

};

#endif // SMARTCAR_EMUL_PERFECTGYROSCOPEI2CSENSOR_HXX
