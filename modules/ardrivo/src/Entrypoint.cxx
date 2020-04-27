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

#include <algorithm>
#include <memory>
#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/view/zip.hpp>
#include "BoardData.hxx"
#include "BoardInfo.hxx"
#include "Entrypoint.hxx"
#include "utility.hxx"

BoardData* board_data = nullptr;
const BoardInfo* board_info = nullptr;

void merge_boards(BoardData old_board, BoardData& new_board) noexcept;

bool init(BoardData* brd, const BoardInfo* info) {
    if (board_data) {
        auto defer_del_bd = std::unique_ptr<BoardData>{board_data};
        auto defer_del_bi = std::unique_ptr<const BoardInfo>{board_info};
        merge_boards(std::move(*board_data), *brd);
    }
    board_data = brd;
    board_info = info;
    return true;
}

void maybe_init() noexcept {
    if (board_data)
        return;

    // over-allocate everything to handle possible IO at dynamic-init
    auto loc_board_data = std::make_unique<BoardData>();
    loc_board_data->silence_errors = true;
    loc_board_data->start_time = std::chrono::steady_clock::now();
    loc_board_data->pin_modes = std::vector<std::atomic_uint8_t>(255);
    loc_board_data->digital_pin_values = std::vector<std::atomic_bool>(255);
    loc_board_data->analog_pin_values = std::vector<std::atomic_uint16_t>(255);
    loc_board_data->i2c_buses = std::vector<I2cBus>(64);
    loc_board_data->uart_buses = std::vector<UartBus>(64);
    loc_board_data->interrupts_handlers = decltype(board_data->interrupts_handlers)(255);
    board_data = loc_board_data.release();

    auto loc_board_info = std::make_unique<BoardInfo>();
    loc_board_info->pins_caps.resize(255);
    loc_board_info->spi_chans.emplace_back();
    ranges::for_each(loc_board_info->pins_caps, [](PinCapability& pin){ pin.digital_in = pin.digital_out = true; }); // default to all digital-able
    // Protocol-pins need not be set as we cannot do pin-locking detection during dynamic-init anyway

    ::board_info = loc_board_info.release();
}

void merge_boards(BoardData old_board, BoardData& new_board) noexcept {
    new_board.start_time = old_board.start_time;
    new_board.interrupts_handlers = std::move(old_board.interrupts_handlers);

    auto copy_atomic_vals = [&]<class T>(std::vector<T>(BoardData::* mvec)) {
            std::transform((old_board.*mvec).begin(), (old_board.*mvec).begin() + (std::min)((old_board.*mvec).size(), (new_board.*mvec).size()),
                           (new_board.*mvec).begin(), []<class U>(const std::atomic<U>& v) -> U { return v; });
        };
    visit(copy_atomic_vals, std::tuple{&BoardData::pin_modes, &BoardData::digital_pin_values, &BoardData::analog_pin_values, &BoardData::pwm_values});

    auto copy_proto_bufs = [&]<class T>(std::vector<T>(BoardData::* mvec)) {
        for(auto [from, to] : ranges::views::zip(old_board.*mvec, new_board.*mvec))
            std::tie(to.rx, to.tx) = std::forward_as_tuple(from.rx, from.tx);
    };
    copy_proto_bufs(&BoardData::uart_buses);

    auto copy_proto_slaves = [&]<class T>(std::vector<T>(BoardData::* mvec)) {
        for(auto [from, to] : ranges::views::zip(old_board.*mvec, new_board.*mvec))
            to.slaves = std::move(from.slaves);
    };
    copy_proto_slaves(&BoardData::i2c_buses);
    copy_proto_slaves(&BoardData::spi_buses);
}