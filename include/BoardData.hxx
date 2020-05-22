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
 *
 */

#ifndef SMARTCAR_EMUL_BOARDDATA_HXX
#define SMARTCAR_EMUL_BOARDDATA_HXX

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>
#include "utility.hxx"

/**
* Protects shared data from being simultaneously accessed by multiple threads
**/
struct BidirMutexes {
    std::mutex rx_mutex;
    std::mutex tx_mutex;
};

/**
* Contains the fixed size of the buffer from transmitter and reciever and its used size
**/
template <std::size_t N>
struct FixBufferBus : BidirMutexes {
    std::array<std::byte, N> rx;
    std::size_t rx_used_size;
    std::array<std::byte, N> tx;
    std::size_t tx_used_size;
};

/**
* Contains the dynamic size of the buffer from transmitter and reciever
**/
struct DynaBufferBus : BidirMutexes {
    std::vector<std::byte> rx;
    std::vector<std::byte> tx;
};

struct PacketBuffers : BidirMutexes {
    std::vector<std::basic_string<std::byte>> rx;
    std::vector<std::basic_string<std::byte>> tx;
};

template <class BufferBus>
struct BlockingBus : BufferBus {
    std::condition_variable sync; // C++20: squash into below atomic by means of `std::atomic::wait`
    std::size_t request_bytes{}; // C++20: use `std::atomic_size_t` for thread-sync
};

struct UartBus : DynaBufferBus {};
struct I2cBus {
    using Device = std::pair<PacketBuffers, std::variant<BlockingBus<std::tuple<>>, std::function<void(std::size_t)>>>;
    std::unordered_map<std::uint8_t, Device> slaves;
    std::mutex devices_mut;
};
struct SpiBus {
    std::unordered_map<std::uint16_t, std::variant<BlockingBus<DynaBufferBus>, std::function<void(std::byte*, std::size_t)>>> slaves; // GCC10: use `std::span` in funcsig
    std::mutex slaves_mut;
};
/**
* Contains the different board data settings 
**/
struct BoardData {
    std::vector<std::atomic_bool> digital_pin_values;
    std::vector<std::atomic_uint16_t> analog_pin_values;
    std::vector<std::atomic_uint8_t> servo_value;
    std::vector<std::atomic_uint8_t> pin_frequency;
    std::vector<UartBus> uart_buses;
    std::vector<I2cBus> i2c_buses;
    std::vector<SpiBus> spi_buses;
    std::vector<std::atomic_uint8_t> pin_modes;

    bool (*write_byte)(unsigned char){};
    size_t (*write_buf)(const unsigned char*, size_t){};
    std::chrono::steady_clock::time_point start_time;

    std::thread::id board_thread_id;
    std::unique_ptr<std::recursive_mutex> interrupt_mut;
    CopyOnMoveAtomic<bool> interrupts = true; /// Whether the board wants interrupts to be fired; devices should check it before firing
    std::vector<std::pair<void (*)(), int>> interrupts_handlers;

    bool silence_errors{};
};
#endif // SMARTCAR_EMUL_BOARDDATA_HXX
