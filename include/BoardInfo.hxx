/*
 *  PinCapabilities.hxx
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

#ifndef SMARTCAR_EMUL_BOARDINFO_HXX
#define SMARTCAR_EMUL_BOARDINFO_HXX

#include <vector>

struct PinCapability {
    bool digital_in : 1;
    bool digital_out : 1;
    bool analog_in : 1  ;
    bool analog_out : 1;
    bool pwm_able : 1;
    bool interrupt_able : 1;
    bool has_alias_name : 1;
};

struct I2cChannelInfo {
    std::uint16_t sda_pin;
    std::uint16_t scl_pin;
};
struct UartChannelInfo {
    std::uint16_t rx_pin;
    std::uint16_t tx_pin;
    std::uint16_t cts_pin; // Clear to send (in)
    std::uint16_t rts_pin; // Request to send (out)
};
struct SpiChannelInfo {
    std::uint16_t mosi_pin{};
    std::uint16_t miso_pin{};
    std::uint16_t clk_pin{};
    std::uint16_t cs_pin{};
};

using PinsCapabilities = std::vector<PinCapability>;

struct BoardInfo {
    PinsCapabilities pins_caps;
    std::vector<I2cChannelInfo> i2c_chans;
    std::vector<UartChannelInfo> uart_chans;
    std::vector<SpiChannelInfo> spi_chans;
};


#endif // SMARTCAR_EMUL_BOARDINFO_HXX
