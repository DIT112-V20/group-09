/*
 *  BoardData.hxx
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

#ifndef SMARTCAR_EMUL_BOARDDATA_HXX
#define SMARTCAR_EMUL_BOARDDATA_HXX

#include <atomic>
#include <vector>
#include <mutex>

struct BidirMutexes {
    std::mutex rx_mutex;
    std::mutex tx_mutex;
};

template <std::size_t N> 
struct FixBufferBus : BidirMutexes {
    std::array<std::byte, N> rx;
    std::size_t rx_used_size;
    std::array<std::byte, N> tx;
    std::size_t tx_used_size;
};

struct DynaBufferBus : BidirMutexes {
    std::vector<std::byte> rx;
    std::vector<std::byte> tx;
};

struct UartBus : DynaBufferBus {};
struct I2cBus : FixBufferBus<32u> {};

struct BoardData {
    std::vector<std::atomic_bool> digital_pin_values;
    std::vector<std::atomic_uint16_t> analog_pin_values;
    std::vector<std::atomic_uint8_t> servo_value;
    std::vector<std::atomic_uint8_t> pin_frequency;
    std::vector<UartBus> uart_buses;
    std::vector<I2cBus> i2c_buses;
    std::vector<std::atomic_uint8_t> pin_modes;

    bool (*write_byte)(unsigned char);
    size_t (*write_buf)(const unsigned char*, size_t);
};
#endif // SMARTCAR_EMUL_BOARDDATA_HXX
