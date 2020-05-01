/*
 *  utility.hxx
 *  Copyright 2020 AeroStun
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
 *
 */
#ifndef SMARTCAR_EMUL_DEVICEMAP_HXX
#define SMARTCAR_EMUL_DEVICEMAP_HXX

#include <array>
#include <gsl/gsl>
#include "BoardData.hxx"

namespace regmon {

enum class MapVals { store = 0, invoke = 1 };

template <class Driver>
struct Device {
    struct DeviceStorage;

    using RegAttachedFunction = void(Driver&, DeviceStorage&, gsl::span<std::byte>);
    using RegAttachedFunctionPtr = RegAttachedFunction*;

    using Storage = std::array<std::uint8_t, 255>;

    struct DeviceMap {
        Storage map{};
        Storage rom{};
        std::array<RegAttachedFunctionPtr, 255> func = {};
    };

    struct DeviceStorage {
        Storage data;
        std::uint8_t address;
    };

    template <std::size_t Reg>
    struct DefaultsTo {
        constexpr static auto register_ = Reg;
        std::uint8_t val;
        void operator()(DeviceMap& dm) { dm.rom[Reg] = std::byte{val}; }
    };
    template <std::size_t Reg>
    struct InvokesFunction {
        constexpr static auto register_ = Reg;
        RegAttachedFunctionPtr func;
        void operator()(DeviceMap& dm) { dm.map[Reg] = MapVals::invoke; dm.func[Reg] = func; }
    };


    template <class... Args> constexpr static auto make_device(Args&&... args) {
        DeviceMap devmap;
        (std::forward<Args>(args)(devmap), ...);
        return devmap;
    }

    constexpr static auto make_ticker(const DeviceMap& devmap, DeviceStorage& storage, I2cBus& bus, Driver& driver) {
        return [&]() {
          std::scoped_lock lock{bus.devices_mut};
          auto& buf = bus.slaves[storage.address].first;
          std::scoped_lock tx_lock{buf.tx_mutex};
          if (buf.tx_used_size > 0) {
              auto tx_off = 1;
              const auto reg = buf.tx.front();
              switch (devmap.map[reg]){
                case MapVals::store:
                  std::memcpy(&storage.data[reg], buf.tx.data() + 1, buf.tx_used_size() -1);
                  break;
                case MapVals::invoke:
                  devmap.func[reg](driver, storage, gsl::make_span(buf.tx.data() + 1, buf.tx_used_size() - 1));
              }
              buf.tx_used_size = 0;
          }
        };
    }
};
}

#endif // SMARTCAR_EMUL_DEVICEMAP_HXX
