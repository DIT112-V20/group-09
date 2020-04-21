/*
 *  BoardConf.hxx
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
 *
 */

#ifndef SMARTCAR_EMUL_BOARDCONF_HXX
#define SMARTCAR_EMUL_BOARDCONF_HXX

#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>
#include <rapidjson/document.h>

namespace smce {

namespace stdfs = std::filesystem;

extern const char* board_conf_schema;

struct BoardConf {
    struct Uart {
        std::uint16_t rx_pin;
        std::uint16_t tx_pin;
    };

    std::string name;
    std::string fqbn; // ArduinoCLI Fully Qualified Board Name
    std::uint16_t digital_pin_count;
    std::uint16_t analog_pin_count;

    std::vector<std::uint16_t> interruptable_digital_pins;
    std::vector<std::uint16_t> pwm_capable_pins;
    std::vector<Uart> uart_pairs;
};

[[nodiscard]] std::optional<BoardConf> load(const rapidjson::Document& json_doc) noexcept;
[[nodiscard]] std::optional<BoardConf> load(const stdfs::path& file_location) noexcept;

}

#endif // SMARTCAR_EMUL_BOARDCONF_HXX
