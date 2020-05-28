#include <thread>
#include <BoardDataDef.hxx>
#include <catch2/catch.hpp>
#include <range/v3/algorithm/equal.hpp>
#include "BoardData.hxx"
#include "BoardInfo.hxx"
#include "Entrypoint.hxx"
#include "Wire.h"

static void init_fake() {
    if (board_data)
        (delete board_data), board_data = nullptr;
    if (board_info)
        (delete board_info), board_data = nullptr;

    auto loc_board_data = std::make_unique<BoardData>();
    loc_board_data->silence_errors = false;
    loc_board_data->start_time = std::chrono::steady_clock::now();
    loc_board_data->pin_modes = std::vector<std::atomic_uint8_t>(3);
    loc_board_data->digital_pin_values = std::vector<std::atomic_bool>(3);
    loc_board_data->i2c_buses = std::vector<I2cBus>(3);

    auto loc_board_info = std::make_unique<BoardInfo>();
    loc_board_info->pins_caps.resize(100);
    loc_board_info->i2c_chans.emplace_back();
    loc_board_info->i2c_chans.emplace_back() = {1, 1};
    loc_board_info->i2c_chans.emplace_back() = {2, 2};

    init(loc_board_data.release(), loc_board_info.release());
}

void reset_wire() {
    Wire.begin(128);
    while (Wire.available()) {
        Wire.read();
    }
}

template<class T>
void read_whole_buf(TwoWire& stream, std::vector<T>& out) {
    std::vector<uint8_t> tmp_buf{};
    while (stream.available()) {
        tmp_buf.push_back(stream.read());
        if (tmp_buf.size() == sizeof(T)) {
            std::memcpy(&out.emplace_back(), tmp_buf.data(), sizeof(T));
            tmp_buf.clear();
        }
    }
}

void slave_worker(const void* input, void* output, size_t stop, PacketBuffers& packet_buffer, BlockingBus<std::tuple<>>& blocking_bus,
                  std::mutex& bus_mutex) {
    const auto* const in = static_cast<const std::byte*>(input);
    auto* const buf = static_cast<std::byte*>(output);

    std::size_t received_bytes = 0;
    std::size_t send_bytes = 0;

    do {
        std::scoped_lock tx_lock{bus_mutex, packet_buffer.tx_mutex};
        if (!packet_buffer.tx.empty()) {
            std::copy(packet_buffer.tx.front().data(), packet_buffer.tx.front().data() + packet_buffer.tx.front().size(),
                      buf + received_bytes);
            received_bytes += packet_buffer.tx.front().size();
            packet_buffer.tx.erase(packet_buffer.tx.begin());
        }
        std::scoped_lock rx_lock{packet_buffer.rx_mutex};
        if (blocking_bus.request_bytes > 0) {
            auto& ret = packet_buffer.rx.emplace_back();
            ret.resize(blocking_bus.request_bytes);
            std::copy(in + send_bytes, in + send_bytes + blocking_bus.request_bytes, ret.begin());
            send_bytes += blocking_bus.request_bytes;
            blocking_bus.request_bytes = 0;
            blocking_bus.sync.notify_all();
        }
    } while (send_bytes < stop);
}

void transfer_test(std::uint8_t bus_no, std:: uint8_t slave_no){
    const auto chunk_size = GENERATE(take(256 / sizeof(unsigned), random(1, 8)));
    const std::vector slave_in = GENERATE_COPY(
        take(1, chunk(chunk_size, random<unsigned>(std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max()))));
    std::vector<decltype(slave_in)::value_type> slave_out(slave_in.size());
    const std::vector master_in = GENERATE_COPY(
        take(1, chunk(chunk_size, random<unsigned>(std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max()))));
    std::vector<decltype(slave_in)::value_type> master_out{};

    auto& blocking_bus = std::get<BlockingBus<std::tuple<>>>(board_data->i2c_buses[bus_no].slaves[slave_no].second);
    auto& packet_buffer = board_data->i2c_buses[bus_no].slaves[slave_no].first;
    auto& bus_mut = board_data->i2c_buses[bus_no].devices_mut;
    board_data->i2c_buses[bus_no].slaves[slave_no].second.emplace<0>();
    auto slave_thread = std::thread(slave_worker, slave_in.data(), slave_out.data(), (size_t)slave_in.size() * sizeof(unsigned), std::ref(packet_buffer), std::ref(blocking_bus), std::ref(bus_mut));

    Wire.begin(bus_no, bus_no);
    Wire.beginTransmission(slave_no);
    Wire.write(reinterpret_cast<const std::uint8_t*>(master_in.data()), master_in.size() * sizeof(unsigned));
    Wire.endTransmission();

    Wire.requestFrom(slave_no, slave_in.size() * sizeof(unsigned), false);
    slave_thread.join();

    read_whole_buf(Wire, master_out);

    REQUIRE(ranges::equal(master_in, slave_out));
    REQUIRE(ranges::equal(slave_in, master_out));
    reset_wire();
}

TEST_CASE("Initiate Wire with or without address", "[Wire Begin]") {
    constexpr std::uint8_t address = 64;
    init_fake();

    // Valid cases
    REQUIRE_NOTHROW(Wire.begin(address));
    REQUIRE_NOTHROW(Wire.begin());
    reset_wire();
}

TEST_CASE("Initiate Wire with sda and scl", "[Wire begin sda&scl]") {
    constexpr std::uint8_t address = 64;
    init_fake();

    // Valid case
    REQUIRE_NOTHROW(Wire.begin(1, 1));
    REQUIRE_NOTHROW(Wire.begin(1, 1, address));
    // SDA or SCL doesn't exist
    REQUIRE_THROWS_AS(Wire.begin(101, 1), std::runtime_error);
    REQUIRE_THROWS_AS(Wire.begin(1, 101), std::runtime_error);
    // SDA SCL pair are not an I2C channel
    REQUIRE_THROWS_AS(Wire.begin(3, 3), std::runtime_error);
    REQUIRE_THROWS_AS(Wire.begin(3, 3, address), std::runtime_error);
    // Invalid address
    REQUIRE_THROWS_AS(Wire.begin(1, 1, 128), std::runtime_error);
    reset_wire();
}

TEST_CASE("Request the data from slave Generator", "[Wire RequestFrom generator]") {
    const auto chunk_size = GENERATE(take(256 / sizeof(unsigned), random(1, 8)));
    const std::vector slave_in = GENERATE_COPY(
        take(1, chunk(chunk_size, random<unsigned>(std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max()))));
    std::vector<decltype(slave_in)::value_type> slave_out(slave_in.size());
    const std::vector master_in = GENERATE_COPY(
        take(1, chunk(chunk_size, random<unsigned>(std::numeric_limits<std::uint8_t>::min(), std::numeric_limits<std::uint8_t>::max()))));

    std::vector<decltype(slave_in)::value_type> master_out{};

    constexpr std::uint8_t address = 64;
    init_fake();

    board_data->i2c_buses[0].slaves[address].second.emplace<1>([&](std::size_t size) mutable {
      auto& tx = board_data->i2c_buses[0].slaves[address].first.tx;
      auto& rx = board_data->i2c_buses[0].slaves[address].first.rx;
      std::memcpy(slave_out.data(), tx.front().data(), tx.front().size());
      tx.erase(tx.begin());
      auto& ret = rx.emplace_back();
      ret.resize(size);
      std::memcpy(ret.data(), slave_in.data(), size);
    });

    Wire.begin();
    Wire.beginTransmission(64);
    Wire.write(reinterpret_cast<const std::uint8_t*>(master_in.data()), master_in.size() * sizeof(unsigned));
    Wire.endTransmission();

    Wire.requestFrom(address, slave_in.size() * sizeof(unsigned), false);

    read_whole_buf(Wire, master_out);

    REQUIRE(ranges::equal(master_in, slave_out));
    REQUIRE(ranges::equal(slave_in, master_out));
    reset_wire();
}

TEST_CASE("Request the data from slave Blocking bus", "[Wire RequestFrom BlockingBus]") {
    init_fake();
    transfer_test(0,64);
}

TEST_CASE("Wire switching between buses", "[Wire BlockingBus]") {
    init_fake();
    const auto bus_no = GENERATE(1, 0, 2);
    const auto slave_no = GENERATE(16, 32, 64);
    transfer_test(bus_no, slave_no);
}

TEST_CASE("Transmission", "[Begin&End transmission]") {
    init_fake();

    // Valid case
    REQUIRE_NOTHROW(Wire.beginTransmission(30));
    // Invalid address
    REQUIRE_THROWS(Wire.beginTransmission(128));
    reset_wire();
}
