/*
 *  PerfectGyroscopeI2C.cxx
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
#include "components/PerfectGyroscopeI2C.hxx"

[[maybe_unused]] [[nodiscard]] bool PerfectGyroscopeI2C::is_powered_up() const noexcept {
    return store.data[CTRL_REG1] & (1u << 3u);
}

[[nodiscard]] auto PerfectGyroscopeI2C::get_power_mode() const noexcept -> PowerMode {
    const std::uint8_t bits = store.data[CTRL_REG1] & 0x0Fu;
    return bits & (1u << 3u) ? (bits & ~(1u << 3u) ? PowerMode::sleeping : PowerMode::normal) : PowerMode::powered_down;
}

[[maybe_unused]] [[nodiscard]] auto PerfectGyroscopeI2C::get_data_rate() const noexcept -> DataRate {
    const std::uint8_t bits = store.data[CTRL_REG1] & 0xF0u;
    const std::uint8_t dr = (bits & 0xCu) >> 2u;
    [[maybe_unused]] const std::uint8_t bw = bits & 0x3u;
    return DataRate{dr};
}

[[nodiscard]] auto PerfectGyroscopeI2C::get_full_scale() const noexcept -> FullScale {
    const std::uint8_t bits = store.data[CTRL_REG4];
    return bits & (1u << 5u) ? FullScale::dps2000 : bits & (1u << 4u) ? FullScale::dps500 : FullScale::dps250;
}

[[nodiscard]] std::endian PerfectGyroscopeI2C::get_data_endianness() const noexcept {
    return ((store.data[CTRL_REG4] >> 6u) & 1u) ? std::endian::big : std::endian::little;
}

PerfectGyroscopeI2C::PerfectGyroscopeI2C(BoardData& bd, Urho3D::Node* node, const rapidjson::Value& pin) : Urho3D::LogicComponent(node->GetContext()) {
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
}

void PerfectGyroscopeI2C::Update(float timeStep) {
    if(get_power_mode() != PowerMode::normal)
        return;

    const auto direction = GetNode()->GetWorldDirection();
    const auto delta_dir = direction - last_direction;
    last_direction = direction;


    auto write_datareg = [&] (auto lreg, auto hreg, std::uint16_t val) {
        if (get_data_endianness() == std::endian::little) {
            store.data[lreg] = val & 0xFu;
            store.data[hreg] = val >> 4u;
        } else {
            store.data[lreg] = val >> 4u;
            store.data[hreg] = val & 0xFu;
        }
    };

    const auto dps = delta_dir / timeStep / fs2sens[static_cast<unsigned>(get_full_scale())];
    if (store.data[CTRL_REG1] & Enabled::x)
        write_datareg(OUT_X_L, OUT_X_H, dps.x_);
    if (store.data[CTRL_REG1] & Enabled::y)
        write_datareg(OUT_Y_L, OUT_Y_H, dps.y_);
    if (store.data[CTRL_REG1] & Enabled::z)
        write_datareg(OUT_Z_L, OUT_Z_H, dps.z_);

    static auto tick = RDev::make_ticker(gy, store, *bus, *this);
    tick();
}