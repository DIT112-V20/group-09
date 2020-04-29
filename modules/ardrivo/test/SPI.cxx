#include <memory>
#include <thread>
#include <BoardDataDef.hxx>
#include <catch2/catch.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include "Arduino.h"
#include "BoardData.hxx"
#include "BoardInfo.hxx"
#include "Entrypoint.hxx"
#include "SPI.h"

void init_fake() {
    if (board_data)
        return;
    auto loc_board_data = std::make_unique<BoardData>();
    loc_board_data->silence_errors = false;
    loc_board_data->start_time = std::chrono::steady_clock::now();
    loc_board_data->pin_modes = std::vector<std::atomic_uint8_t>(2);
    loc_board_data->digital_pin_values = std::vector<std::atomic_bool>(2);
    loc_board_data->spi_buses = std::vector<SpiBus>(3);

    auto loc_board_info = std::make_unique<BoardInfo>();
    loc_board_info->pins_caps.resize(3);
    loc_board_info->spi_chans.emplace_back();
    loc_board_info->spi_chans.emplace_back() = {1, 1, 1, 1};
    loc_board_info->spi_chans.emplace_back() = {2, 2, 2, 2};
    ranges::for_each(&loc_board_info->pins_caps.front(), &loc_board_info->pins_caps[10],
                     [](PinCapability& pin) { pin.digital_in = pin.digital_out = true; });

    init(loc_board_data.release(), loc_board_info.release());
}

void slave_worker(const void* input, void* output, size_t stop, BlockingBus<DynaBufferBus>& bus_buffer) {
    const auto* in = static_cast<const std::byte*>(input);
    auto* buf = static_cast<std::byte*>(output);

    size_t received_bytes = 0;
    size_t send_bytes = 0;
    do {
        std::scoped_lock lock{bus_buffer.rx_mutex, bus_buffer.tx_mutex, bus_buffer.request_bytes_mutex};
        if (!bus_buffer.tx.empty()) {
            std::copy(bus_buffer.tx.begin(), bus_buffer.tx.begin() + send_bytes, buf + received_bytes);
            received_bytes += send_bytes;
            bus_buffer.tx.clear();
        }
        if (bus_buffer.request_bytes > 0) {
            bus_buffer.rx.resize(bus_buffer.request_bytes);
            std::copy(in + received_bytes, in + received_bytes + bus_buffer.request_bytes, bus_buffer.rx.begin());
            send_bytes = bus_buffer.request_bytes;
            bus_buffer.request_bytes = 0;
            bus_buffer.sync.notify_all();
        }
    } while (received_bytes < stop);
};

void transfer_test(size_t bus_no, size_t slave_no) {
    board_data->spi_buses[bus_no].slaves[slave_no].emplace<BlockingBus<DynaBufferBus>>();
    auto& bus_buffer = std::get<BlockingBus<DynaBufferBus>>(board_data->spi_buses[bus_no].slaves[slave_no]);

    // Setup a slave with bytes it can send to the master
    std::vector<std::uint8_t> slave_out{0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFA, 0x2C};
    std::vector<std::uint8_t> slave_buf_vec(slave_out.size());

    auto slave_thread = std::thread(slave_worker, slave_out.data(), slave_buf_vec.data(), slave_buf_vec.size(), std::ref(bus_buffer));

    REQUIRE(SPI.transfer(0x2C) == 0x2C);
    SPI.begin(slave_no);

    // Check master receiver proper bytes
    std::vector<std::uint8_t> master_out{0xFA, 0xF0, 0xF0, 0xFA, 0xFA};

    for (size_t i = 0; i < master_out.size(); ++i) {
        REQUIRE(slave_out[i] == SPI.transfer(master_out[i]));
    }
    // Check 2 byte transfer
    constexpr uint16_t bit16 = 0x00FF;
    SPI.transfer16(bit16);

    slave_thread.join();
    // Copy over 2 byte value into master_out
    master_out.resize(master_out.size() + sizeof(bit16));
    std::memcpy(&master_out.back() - 1, &bit16, sizeof(bit16));
    // Check slave received proper bytes
    REQUIRE(ranges::equal(slave_buf_vec, master_out));
    SPI.end();
}
TEST_CASE("SPI multi transfer + same slave", "[SPI mixed transfer]") {
    init_fake();
    transfer_test(0, 0);
}

TEST_CASE("SPI exceptions", "[SPI failures]") {
    init_fake();
    CHECK_THROWS(SPI.begin(11));
    CHECK_THROWS(SPI.begin(999));
    CHECK_THROWS(SPI.pins(21, 0, 0, 0));
    CHECK_THROWS(SPI.pins(0, 21, 0, 0));
    CHECK_THROWS(SPI.pins(0, 0, 21, 0));
    CHECK_THROWS(SPI.pins(0, 0, 0, 21));
    CHECK_THROWS(SPI.pins(0, 0, 0, 11));
}

TEST_CASE("SPI switching between busses", "[SPI bus change]") {
    init_fake();
    REQUIRE(SPI.pins(1, 1, 1, 1));
    transfer_test(1, 1);
    REQUIRE(SPI.pins(0, 0, 0, 0));
    transfer_test(0, 0);
}

TEST_CASE("SPI generator function", "[SPI generator]") {
    init_fake();
    SPI.pins(2, 2, 2, 2);
    SPI.begin(2);
    std::vector<uint8_t> master_ref{0x2C, 0xF0};
    std::vector<uint8_t> master_buf = master_ref;
    std::vector<uint8_t> slave_buf(master_buf.size());
    std::vector<uint8_t> slave_ref(master_buf.size());
    uint8_t i{};
    std::generate(slave_ref.begin(), slave_ref.end(), [&]() { return ++i; });
    board_data->spi_buses[2].slaves[2].emplace<std::function<void(std::byte*, std::size_t)>>([&](std::byte* buf, std::size_t size) {
        std::memcpy(slave_buf.data(), buf, size);
        uint8_t j{};
        std::generate(buf, buf + size, [&]() { return static_cast<std::byte>(++j); });
    });
    SPI.transfer(master_buf.data(), master_buf.size());
    REQUIRE(ranges::equal(slave_buf, master_ref));
    REQUIRE(ranges::equal(master_buf, slave_ref));
}
