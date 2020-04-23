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
#include "BoardData.hxx"
#include "BoardInfo.hxx"

namespace smce {

namespace stdfs = std::filesystem;

extern const char* board_conf_schema;

struct BoardConf {
    struct I2c {
        std::uint16_t sda_pin;
        std::uint16_t scl_pin;
    };
    struct Uart {
        std::uint16_t rx_pin;
        std::uint16_t tx_pin;
        std::uint16_t cts_pin; // Clear to send (in)
        std::uint16_t rts_pin; // Request to send (out)
    };
    struct Spi {
        std::uint16_t mosi_pin;
        std::uint16_t miso_pin;
        std::uint16_t clk_pin;
        std::uint16_t cs_pin;
    };

    std::string name;
    std::string fqbn; // ArduinoCLI Fully Qualified Board Name
    std::uint16_t pin_count;

    std::vector<std::uint16_t> digital_in_unable_pins;
    std::vector<std::uint16_t> digital_out_unable_pins;
    std::vector<std::uint16_t> analog_in_capable_pins;
    std::vector<std::uint16_t> analog_out_capable_pins;
    std::vector<std::uint16_t> pwm_capable_pins;
    std::vector<std::uint16_t> interruptable_pins;
    std::vector<Uart> uart_quads;
    std::vector<I2c> i2c_pairs;
    std::vector<Spi> spi_quads;
};

[[nodiscard]] std::optional<BoardConf> load(const rapidjson::Document& json_doc) noexcept;
[[nodiscard]] std::optional<BoardConf> load(const stdfs::path& file_location) noexcept;
[[nodiscard]] BoardData as_data(const BoardConf& conf) noexcept;
[[nodiscard]] BoardInfo as_info(const BoardConf& conf) noexcept;

}

#endif // SMARTCAR_EMUL_BOARDCONF_HXX
