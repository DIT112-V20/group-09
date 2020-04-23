/*
 *  Entrypoint.cxx
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

#include <memory>
#include <range/v3/algorithm/for_each.hpp>
#include "BoardData.hxx"
#include "BoardInfo.hxx"
#include "Entrypoint.hxx"

BoardData* board_data = nullptr;
const BoardInfo* board_info = nullptr;

bool init(BoardData* brd, const BoardInfo* info) {
    if (board_data) {
        auto defer_del_bd = std::unique_ptr<BoardData>{board_data};
        auto defer_del_bi = std::unique_ptr<const BoardInfo>{board_info};
        delete board_info;
        //merge_config();
    }
    board_data = brd;
    board_info = info;
    return true;
}

void maybe_init() noexcept {
    if (board_data)
        return;

    // over-allocate everything to handle possible IO at dynamic-init
    board_data = new BoardData;
    board_data->silence_errors = true;
    board_data->start_time = std::chrono::steady_clock::now();
    board_data->pin_modes = std::vector<std::atomic_uint8_t>(255);
    board_data->digital_pin_values = std::vector<std::atomic_bool>(255);
    board_data->analog_pin_values = std::vector<std::atomic_uint16_t>(255);
    board_data->i2c_buses = std::vector<I2cBus>(64);
    board_data->uart_buses = std::vector<UartBus>(64);
    board_data->interrupts_handlers = decltype(board_data->interrupts_handlers)(255);

    BoardInfo* loc_board_info = new BoardInfo;
    loc_board_info->pins_caps.resize(255);
    ranges::for_each(loc_board_info->pins_caps, [](PinCapability& pin){ pin.digital_in = pin.digital_out = true; }); // default to all digital-able
    // Protocol-pins need not be set as we cannot do pin-locking detection during dynamic-init anyway

    ::board_info = loc_board_info;
}