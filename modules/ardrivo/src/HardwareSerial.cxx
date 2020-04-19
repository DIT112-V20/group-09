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
#include "HardwareSerial.h"

void HardwareSerial::begin(unsigned long, uint8_t) { begun = true; }

void HardwareSerial::end() { begun = false; }

int HardwareSerial::available() { return std::numeric_limits<int>::max(); }

int HardwareSerial::availableForWrite() { return std::numeric_limits<int>::max(); }

size_t HardwareSerial::write(uint8_t c) {
    if (!begun)
        return 0;
    board_data->write_byte(c);
    return 1;
}

size_t HardwareSerial::write(const uint8_t* buf, std::size_t n) {
    if (!begun)
        return 0;
    board_data->write_buf(buf, n);
    return n;
}

int HardwareSerial::peek() {
    if (!begun)
        return -1;
    std::lock_guard guard{board_data->uart_rx_buf_mutex};
    return board_data->uart_rx_buf.front();
}

int HardwareSerial::read() {
    if (!begun)
        return -1;
    std::lock_guard guard{board_data->uart_rx_buf_mutex};
    int firstByte = +board_data->uart_rx_buf.front();
    board_data->uart_rx_buf.erase(board_data->uart_rx_buf.begin());
    return firstByte;
}
