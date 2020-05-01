#include <limits>
#include <memory>
#include <thread>
#include <valarray>
#include <BoardDataDef.hxx>
#include <catch2/catch.hpp>
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
    loc_board_data->pin_modes = std::vector<std::atomic_uint8_t>(3);
    loc_board_data->digital_pin_values = std::vector<std::atomic_bool>(3);
    loc_board_data->spi_buses = std::vector<SpiBus>(3);

    auto loc_board_info = std::make_unique<BoardInfo>();
    loc_board_info->pins_caps.resize(3);
    loc_board_info->spi_chans.emplace_back();
    loc_board_info->spi_chans.emplace_back() = {1, 1, 1, 1};
    loc_board_info->spi_chans.emplace_back() = {2, 2, 2, 2};
    ranges::for_each(loc_board_info->pins_caps,
                     [](PinCapability& pin) { pin.digital_in = pin.digital_out = true; });

    init(loc_board_data.release(), loc_board_info.release());
}

void slave_worker(const void* input, void* output, size_t stop, BlockingBus<DynaBufferBus>& bus_buffer) {
    const auto* const in = static_cast<const std::byte*>(input);
    auto* const buf = static_cast<std::byte*>(output);

    std::size_t received_bytes = 0;
    std::size_t send_bytes = 0;
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

void transfer_test(std::size_t bus_no, std::size_t slave_no) {
    board_data->spi_buses[bus_no].slaves[slave_no].emplace<BlockingBus<DynaBufferBus>>();
    auto& bus_buffer = std::get<BlockingBus<DynaBufferBus>>(board_data->spi_buses[bus_no].slaves[slave_no]);

    const auto& bus_info = board_info->spi_chans[bus_no];
    SPI.pins(bus_info.clk_pin, bus_info.mosi_pin, bus_info.miso_pin, slave_no);
    SPI.begin(slave_no);

    SECTION("Single byte transfer"){
        const std::uint8_t slave_in = GENERATE(range(std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max()));
        std::uint8_t slave_out{};
        auto slave_thread = std::thread(slave_worker, &slave_in, &slave_out, sizeof(slave_in), std::ref(bus_buffer));

        const std::uint8_t master_in = GENERATE(range(std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max()));
        const std::uint8_t master_out = SPI.transfer(master_in);
        slave_thread.join();

        REQUIRE(slave_out == master_in);
        REQUIRE(master_out == slave_in);
    }

    SECTION("Twin bytes transfer") {
        const auto slave_ins = GENERATE(take(1, chunk(256, random<unsigned>(std::numeric_limits<std::uint16_t>::min(), std::numeric_limits<std::uint16_t>::max()))));
        const auto master_ins = GENERATE(take(1, chunk(256, random<unsigned>(std::numeric_limits<std::uint16_t>::min(), std::numeric_limits<std::uint16_t>::max()))));
        for(const std::uint16_t slave_in : slave_ins) {
            for(const std::uint16_t master_in : master_ins) {
                std::uint16_t slave_out{};
                auto slave_thread = std::thread(slave_worker, &slave_in, &slave_out, sizeof(slave_in), std::ref(bus_buffer));

                const std::uint16_t master_out = SPI.transfer16(master_in);
                slave_thread.join();

                REQUIRE(slave_out == master_in);
                REQUIRE(master_out == slave_in);
            }
        }
    }

    SECTION("Multibyte transfer") {
        const auto chunk_size = GENERATE(take(256 / sizeof(unsigned), random(1, 16384)));
        const std::vector slave_in = GENERATE_COPY(take(1, chunk(chunk_size, random<unsigned>(std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max()))));
        std::vector<decltype(slave_in)::value_type> slave_out(slave_in.size());
        auto slave_thread = std::thread(slave_worker, slave_in.data(), slave_out.data(), slave_in.size(), std::ref(bus_buffer));

        const std::vector master_in = GENERATE_COPY(take(1, chunk(chunk_size, random<unsigned>(std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max()))));;
        auto master_out = master_in;
        SPI.transfer(master_out.data(), chunk_size * sizeof(unsigned));
        slave_thread.join();

        REQUIRE(slave_out == master_in);
        REQUIRE(master_out == slave_in);
    }

    SPI.end();
}
TEST_CASE("SPI multi transfer + same slave", "[SPI mixed transfer]") {
    init_fake();
    transfer_test(0, 0);
}

TEST_CASE("SPI exceptions", "[SPI failures]") {
    init_fake();
    CHECK_THROWS(SPI.begin(GENERATE(range(4, 255)))); // SPI has a call to `pinMode` which takes the pin as a `uint8_t`
    std::valarray args{{GENERATE(range(4, 255)), 0, 0, 0}};
    args.cshift(GENERATE(0, 1, 2, 3));
    CHECK_THROWS(SPI.pins(args[0], args[1], args[2], args[3]));
}

TEST_CASE("SPI switching between buses", "[SPI bus change]") {
    init_fake();
    const auto bus_no = GENERATE(1, 0, 2);
    const auto slave_no = GENERATE(0, 1, 2);
    transfer_test(bus_no, slave_no);
}

TEST_CASE("SPI generator function", "[SPI generator]") {
    init_fake();
    SPI.pins(2, 2, 2, 2);
    SPI.begin(2);

    const auto chunk_size = GENERATE(take(256 / sizeof(unsigned), random(1, 16384)));
    const std::vector slave_in = GENERATE_COPY(take(1, chunk(chunk_size, random<unsigned>(std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max()))));
    std::vector<decltype(slave_in)::value_type> slave_out(slave_in.size());
    const std::vector master_in = GENERATE_COPY(take(1, chunk(chunk_size, random<unsigned>(std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max()))));;
    std::vector master_out = master_in;

    board_data->spi_buses[2].slaves[2].emplace<1>([&](std::byte* buf, std::size_t size) {
        std::memcpy(slave_out.data(), buf, size);
        std::memcpy(buf, slave_in.data(), size);
    });

    SPI.transfer(master_out.data(), master_out.size() * sizeof(unsigned ));

    REQUIRE(slave_out == master_in);
    REQUIRE(master_out == slave_in);
}
