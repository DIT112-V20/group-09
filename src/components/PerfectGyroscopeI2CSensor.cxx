/*
 *  PerfectGyroscopeI2CSensor.cxx
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

#include <cstdint>
#include <Urho3D/Scene/Node.h>
#include "components/PerfectGyroscopeI2CSensor.hxx"
#include <Urho3D/Physics/PhysicsEvents.h>

[[maybe_unused]] [[nodiscard]] bool PerfectGyroscopeI2CSensor::is_powered_up() const noexcept {
    return store.data[CTRL_REG1] & (1u << 3u);
}

[[nodiscard]] auto PerfectGyroscopeI2CSensor::get_power_mode() const noexcept -> PowerMode {
    const std::uint8_t bits = store.data[CTRL_REG1] & 0x0Fu;
    return bits & (1u << 3u) ? (bits & 7u ? PowerMode::normal : PowerMode::sleeping) : PowerMode::powered_down;
}

[[maybe_unused]] [[nodiscard]] auto PerfectGyroscopeI2CSensor::get_data_rate() const noexcept -> DataRate {
    const std::uint8_t bits = store.data[CTRL_REG1] & 0xF0u;
    const std::uint8_t dr = (bits & 0xCu) >> 2u;
    [[maybe_unused]] const std::uint8_t bw = bits & 0x3u;
    return DataRate{dr};
}

[[nodiscard]] auto PerfectGyroscopeI2CSensor::get_full_scale() const noexcept -> FullScale {
    const std::uint8_t bits = store.data[CTRL_REG4];
    return bits & (1u << 5u) ? FullScale::dps2000 : bits & (1u << 4u) ? FullScale::dps500 : FullScale::dps250;
}

[[nodiscard]] std::endian PerfectGyroscopeI2CSensor::get_data_endianness() const noexcept {
    return ((store.data[CTRL_REG4] >> 6u) & 1u) ? std::endian::big : std::endian::little;
}

PerfectGyroscopeI2CSensor::PerfectGyroscopeI2CSensor(BoardData& bd, Urho3D::Node* node, const rapidjson::Value& pin) : Urho3D::LogicComponent(node->GetContext()), ticker{} {
    SetUpdateEventMask(Urho3D::USE_FIXEDUPDATE | Urho3D::USE_POSTUPDATE);
    SubscribeToEvent(Urho3D::E_PHYSICSPOSTSTEP, URHO3D_HANDLER(PerfectGyroscopeI2CSensor, UpdatePy));

    if (!pin.HasMember("bus_id") || !pin.HasMember("address"))
        throw std::runtime_error{"Attempted to create component PerfectGyroscopeI2C with an invalid configuration"};

    std::uint8_t bus_id = pin["bus_id"].GetUint();
    store.address = pin["address"].GetUint();
    store.data = gy.rom;
    bus = &bd.i2c_buses[bus_id];
    std::scoped_lock lock{bus->devices_mut};
    bus->slaves[store.address].second.emplace<1>([&](std::size_t size) mutable {
      auto& buf = bus->slaves[store.address].first;

      auto reg = static_cast<uint8_t>(buf.tx.back().front());
      buf.tx.erase(buf.tx.end() - 1);

      std::basic_string<std::byte> ret{};
      ret.resize(size);
      // Prevent reading beyond the ram
      if (reg + size > store.data.size()) {
          buf.rx.emplace_back(ret);
          return;
      }
      // Place requested bytes into rx buffer
      auto& s = store.data;
      std::memcpy(ret.data(), &store.data[reg], size);
      buf.rx.emplace_back(ret);
    });

    ticker = RDev::make_ticker(gy, store, *bus, *this);
}

void PerfectGyroscopeI2CSensor::UpdatePy(Urho3D::StringHash, Urho3D::VariantMap& event_data) {
    ticker();
    if(get_power_mode() != PowerMode::normal)
        return;

    // For now we patch smartcar_lib to accept straight absolute angles
    const auto direction = node_->GetWorldRotation().EulerAngles();

    auto write_datareg = [&] (auto lreg, auto hreg, std::int16_t val) {
      val += 180;
      if (get_data_endianness() == std::endian::little) {
          store.data[lreg] = val & 0xFFu;
          store.data[hreg] = val >> 8u;
      } else {
          store.data[lreg] = val >> 8u;
          store.data[hreg] = val & 0xFFu;
      }
    };

    if (store.data[CTRL_REG1] & Enabled::x)
        write_datareg(OUT_X_L, OUT_X_H, direction.z_);
    if (store.data[CTRL_REG1] & Enabled::y)
        write_datareg(OUT_Y_L, OUT_Y_H, direction.x_);
    if (store.data[CTRL_REG1] & Enabled::z)
        write_datareg(OUT_Z_L, OUT_Z_H, direction.y_);
}
