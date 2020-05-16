/*
 *  SPI.cxx
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

#include <algorithm>
#include <fmt/format.h>
#include <range/v3/algorithm/find_if.hpp>
#include "Arduino.h"
#include "BoardDataDef.hxx"
#include "Entrypoint.hxx"
#include "Error.hxx"
#include "SPI.h"
#include "utility.hxx"

SPIClass SPI;

void SPIClass::begin(){
    if (!board_info)
        return; // CAVEAT: [dynamic-init] `SPI.begin()` is a no-op
    active = true;
    if(slave_sel < 0)
        slave_sel = board_info->spi_chans[bus_id].cs_pin;
    pinMode(slave_sel, static_cast<std::uint8_t>(PinMode::OUTPUT));
}

void SPIClass::begin(int slave_select) {
    maybe_init();
    const auto debug_sig = [=](const char* msg) { return fmt::format("SPIClass::begin({}): {}", slave_select, msg); };
    if (slave_select >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"));
    if (!board_info->pins_caps[slave_select].digital_out)
        return handle_error(debug_sig("Pin cannot be used as SPI SS since it cannot handle digital output"));
    pinMode(slave_select, static_cast<std::uint8_t>(PinMode::OUTPUT));
    active = true;
    slave_sel = slave_select;
}

void SPIClass::end() {
    active = false;
}

void SPIClass::beginTransaction(SPISettings spi_settings){
    if(!active)
        return;
    digitalWrite(slave_sel, true);
}

void SPIClass::endTransaction() {
    if(!active)
        return;
    digitalWrite(slave_sel, false);
}

void SPIClass::setBitOrder(SMCE__BitOrdering bit_ordering) {
    settings.bit_order = bit_ordering;
}

void SPIClass::setDataMode(SMCE__SpiMode spi_mode) {
    settings.data_mode = spi_mode;
}

void SPIClass::setDataMode(int slave_select, std::uint8_t spi_mode) {
    maybe_init();
    const auto debug_sig = [=](const char* msg) { return fmt::format("SPIClass::setDataMode({}, {}): {}", slave_select, spi_mode, msg); };
    if (slave_select >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"));
    if (!board_info->pins_caps[slave_select].digital_out)
        return handle_error(debug_sig("Pin cannot be used as SPI SS since it cannot handle digital output"));
    settings.data_mode = static_cast<SMCE__SpiMode>(spi_mode);
}

void SPIClass::setClockDivider(SMCE__SpiClockDivider divider) {
    settings.clock_div = divider;
}

void SPIClass::setClockDivider(int slave_select, std::uint8_t divider) {
    maybe_init();
    const auto debug_sig = [=](const char* msg) { return fmt::format("SPIClass::setClockDivider({}, {}): {}", slave_select, divider, msg); };
    if (slave_select >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"));
    if (!board_info->pins_caps[slave_select].digital_out)
        return handle_error(debug_sig("Pin cannot be used as SPI SS since it cannot handle digital output"));
    settings.clock_div = static_cast<SMCE__SpiClockDivider>(divider);
}

uint8_t SPIClass::transfer(std::uint8_t data) {
    transfer(&data, sizeof(data));
    return data;
}

uint16_t SPIClass::transfer16(std::uint16_t data) {
    transfer(&data, sizeof(data));
    return data;
}

// [Precond: (buf || count == 0)]
void SPIClass::transfer(void* buf, std::uint16_t count) {
    if(!active || count == 0)
        return;
    std::lock_guard lk{*board_data->interrupt_mut}; // CAVEAT: [all] SPI transfers temporarily inhibit interrupts
    // Assume that `this->active` can only be truthy if `maybe_init` has been called
    std::unique_lock lock{::board_data->spi_buses[bus_id].slaves_mut};
    auto& slave = ::board_data->spi_buses[bus_id].slaves[slave_sel];
    if(slave.valueless_by_exception()) // Device not initialized -- dynamic-init
        return; // CAVEAT: [dynamic-init] SPI.transfer(/*...*/) is a no-op

    auto* byte_buf = static_cast<std::byte*>(buf);
    std::visit(Visitor{
                   [=, processed = 0ull, left = static_cast<std::size_t>(count), &lock](BlockingBus<DynaBufferBus>& bus_buffer) mutable {
                       do {

                           {
                               std::scoped_lock lock{bus_buffer.rx_mutex, bus_buffer.tx_mutex};
                               const auto swp_len = (std::min)(left, bus_buffer.rx.size());
                               const auto old_size = bus_buffer.tx.size();
                               bus_buffer.tx.resize(bus_buffer.tx.size() + swp_len);
                               std::copy(byte_buf + processed, byte_buf + swp_len, bus_buffer.tx.begin() + old_size);
                               std::copy(bus_buffer.rx.begin(), bus_buffer.rx.begin() + swp_len, byte_buf + processed);
                               bus_buffer.rx.erase(bus_buffer.rx.begin(), bus_buffer.rx.begin() + swp_len);
                               processed += swp_len;
                               left -= swp_len;
                           }
                           if (left == 0)
                               break;

                           bus_buffer.request_bytes = left;
                           bus_buffer.sync.wait(lock);
                       } while(true);

                   },
                   [=](const std::function<void(std::byte*, std::size_t)>& generator){
                       std::lock_guard lk{*board_data->interrupt_mut};
                       generator(static_cast<std::byte*>(buf), count);
                   }
               }, slave);
}

bool SPIClass::pins(std::int8_t sck, std::int8_t miso, std::int8_t mosi, std::int8_t ss) {
    maybe_init();
    const auto debug_sig = [=](const char* msg) { return fmt::format("SPIClass::pins({}, {}, {}, {}): {}", sck, miso, mosi, ss, msg); };
    if (sck >= board_info->pins_caps.size())
        return handle_error(debug_sig("SCK pin does not exist"), false);
    if (miso >= board_info->pins_caps.size())
        return handle_error(debug_sig("MISO pin does not exist"), false);
    if (mosi >= board_info->pins_caps.size())
        return handle_error(debug_sig("MOSI pin does not exist"), false);
    if (ss >= board_info->pins_caps.size())
        return handle_error(debug_sig("SS pin does not exist"), false);

    const auto it = ranges::find_if(board_info->spi_chans, [=](const auto& chan){ return std::tie(chan.clk_pin, chan.miso_pin, chan.mosi_pin) == std::tuple{sck, miso, mosi}; });
    if(it == board_info->spi_chans.end())
        return false;

    bus_id = std::distance(board_info->spi_chans.begin(), it);

    if (!board_info->pins_caps[ss].digital_out)
        return handle_error(debug_sig("Pin cannot be used as SPI SS since it cannot handle digital output"), false);
    if (active)
        pinMode(ss, static_cast<std::uint8_t>(PinMode::OUTPUT));
    return true;
}