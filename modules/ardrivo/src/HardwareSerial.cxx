/*
 *  HardwareSerial.cxx
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

#include <iostream>
#include <limits>
#include "BoardDataDef.hxx"
#include "Entrypoint.hxx"
#include "HardwareSerial.h"

HardwareSerial Serial;
#ifdef _MSC_VER
HardwareSerial& SMCE__DATA_TRAMPOLINE_Serial() noexcept { return Serial; }
#endif

void HardwareSerial::begin(unsigned long, uint8_t) {
    maybe_init();
    begun = true;
}

void HardwareSerial::end() { begun = false; }

int HardwareSerial::available() {
    std::scoped_lock lock{board_data->uart_buses[0].rx_mutex};
    return board_data->uart_buses[0].rx.size();
}

int HardwareSerial::availableForWrite() { return std::numeric_limits<int>::max(); }

size_t HardwareSerial::write(uint8_t c) {
    if (!begun || !board_data->write_byte)
        return 0;
    std::lock_guard lk{*board_data->interrupt_mut};
    board_data->write_byte(c);
    return 1;
}

size_t HardwareSerial::write(const uint8_t* buf, std::size_t n) {
    if (!begun || !board_data->write_buf)
        return 0;
    std::lock_guard lk{*board_data->interrupt_mut};
    board_data->write_buf(buf, n);
    return n;
}

int HardwareSerial::peek() {
    if (!begun || available() == 0)
        return -1;
    std::lock_guard guard{board_data->uart_buses[0].rx_mutex};
    return static_cast<int>(board_data->uart_buses[0].rx.front());
}

int HardwareSerial::read() {
    if (!begun || available() == 0)
        return -1;
    std::lock_guard guard{board_data->uart_buses[0].rx_mutex};
    const int first_byte = static_cast<int>(board_data->uart_buses[0].rx.front());
    board_data->uart_buses[0].rx.erase(board_data->uart_buses[0].rx.begin());
    return first_byte;
}
