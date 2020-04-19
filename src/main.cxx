/*
 *  main.cxx
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

#include "utility.hxx"
#include "Toolchain.hxx"
#include "Runtime.hxx"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <sstream>
#include <fmt/format.h>

int main(int argc, char** argv) {
    if (argc < 2) { // no args
        std::cout << fmt::format("Usage: {} path/to/sketch.ino", argv[0]) << std::endl;
        return -1;
    }

    std::filesystem::path source = argv[1];
    if(argv[1][0] == '.')
        source = std::filesystem::current_path() / argv[1];
    std::cout << "File: " << source << std::endl;
    auto compile_res = smce::compile_sketch({source}, ".");
    auto sketch_obj = std::visit(Visitor{
        [](const std::runtime_error&) -> smce::SketchObject&& {
            std::cerr << "[FATAL] Compilation failed" << std::endl;
            std::exit(EXIT_FAILURE);
        },
        [](smce::SketchObject& obj) -> decltype(auto) { return std::move(obj); }
    }, compile_res);

    BoardData bd;
    auto to_uart = [&](std::string str) {
        std::scoped_lock l{bd.uart_buses[0].rx_mutex};
        const auto original_buf_len = bd.uart_buses[0].rx.size();
        bd.uart_buses[0].rx.resize(bd.uart_buses[0].rx.size() + str.size());
        std::transform(str.begin(), str.end(), bd.uart_buses[0].rx.begin() + original_buf_len, [](char c){ return static_cast<std::byte>(c); });
    };

    bd.interrupts_handlers = {};
    bd.uart_buses = std::vector<UartBus>(1);
    bd.digital_pin_values = std::vector<std::atomic_bool>(5);
    bd.analog_pin_values = std::vector<std::atomic_ushort>(5);
    bd.write_byte = +[](unsigned char c){ return static_cast<bool>(std::cout.put(c)); };
    bd.write_buf = +[](const unsigned char* buf, std::size_t len){ return std::cout.write(reinterpret_cast<const char*>(buf), len) ? len : 0; };
    smce::SketchRuntime runtime;
    if(!runtime.set_sketch_and_car(sketch_obj, bd)) {
        std::cerr << "[FATAL] Initializing failed\n" << std::flush;
        return EXIT_FAILURE;
    }
    std::cout << "[INFO] Initialized\n" << std::flush;

    bd.start_time = std::chrono::steady_clock::now();
    runtime.start();

    while (true) {
        std::cout << "$> " << std::flush;
        std::string input;
        std::getline(std::cin, input);
        if(std::cin.rdbuf()->in_avail())
            std::cin.ignore(-1);
        if(input == "\\q") {
            runtime.pause_on_next_loop();
            break;
        }
        if(input == "\\p") {
            runtime.pause_on_next_loop();
            continue;
        }
        if(input == "\\r") {
            runtime.resume();
            continue;
        }
        if(input.starts_with("\\r ")) {
            const auto pin = std::stoi(input.substr(2));
            if(pin < 0)
                std::cout << fmt::format("$[{}]> {}\n", -pin, bd.analog_pin_values[-pin]) << std::flush;
            else
                std::cout << fmt::format("$[{}]> {}\n", pin, bd.digital_pin_values[pin]) << std::flush;

            continue;
        }
        if(input.starts_with("\\w ")) {
            std::stringstream ss{input.substr(2)};
            int pin{};
            unsigned short val{};
            ss >> pin >> val;
            if(pin < 0)
                bd.analog_pin_values[pin *= -1] = val;
            else
                bd.digital_pin_values[pin] = val;
            std::cout << fmt::format("$[{}]> {}\n", pin, val) << std::flush;
            continue;
        }
        to_uart(input);
    }
}