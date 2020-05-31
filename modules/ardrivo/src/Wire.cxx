#include <cstring>
#include <fmt/format.h>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include "BoardDataDef.hxx"
#include "Entrypoint.hxx"
#include "Error.hxx"
#include "Wire.h"

TwoWire Wire;
#ifdef _MSC_VER
TwoWire& SMCE__DATA_TRAMPOLINE_Wire() noexcept { return Wire; }
#endif

void TwoWire::begin() { begun = true; }

void TwoWire::begin(std::uint8_t with_address) {
    begin();
    own_address = with_address;
}

void TwoWire::begin(int sda, int scl) {
    maybe_init();
    const auto debug_sig = [=](const char* msg) { return fmt::format("Wire.begin({}, {}): {}", sda, scl, msg); };
    if (sda >= board_info->pins_caps.size())
        return handle_error(debug_sig("SDA pin does not exist"));
    if (scl >= board_info->pins_caps.size())
        return handle_error(debug_sig("SDL pin does not exist"));

    const auto it = ranges::find_if(board_info->i2c_chans, [=](const I2cChannelInfo& chan) {
        return std::pair{+chan.sda_pin, +chan.scl_pin} == std::pair{sda, scl};
    });
    if (it == board_info->i2c_chans.end())
        return handle_error("SDA SCL pair are not an I2C channel");

    begin();
    bus_id = std::distance(board_info->i2c_chans.begin(), it);
}

void TwoWire::begin(int sda, int scl, std::uint8_t with_address) {
    maybe_init();
    const auto debug_sig = [=](const char* msg) { return fmt::format("Wire.begin({}, {}, {}): {}", sda, scl, +with_address, msg); };
    if (with_address & no_address)
        handle_error(debug_sig("Invalid address"));
    begin(sda, scl);
    begin(with_address);
}

std::size_t TwoWire::requestFrom(std::uint8_t address, std::size_t quantity, [[maybe_unused]] bool stop) {
    if (!begun || quantity == 0)
        return 0;
    maybe_init();
    const auto debug_sig = [=](const char* msg) { return fmt::format("Wire.requestFrom({}, {}, {}): {}", +address, quantity, stop, msg); };
    if (address & no_address)
        handle_error(debug_sig("Invalid address"));

    std::unique_lock lock{board_data->i2c_buses[bus_id].devices_mut};
    auto& [device_buf, device] = board_data->i2c_buses[bus_id].slaves[address];

    if (device.valueless_by_exception()) // not initialized -- dynamic init or device got moved
        return 0;                        // CAVEAT: [dynamic-init] TWI IO is a no-op at dynamic init

    std::visit(Visitor{
                   [quantity, &lock](BlockingBus<std::tuple<>>& blocking_bus) mutable {
                       blocking_bus.request_bytes = quantity;
                       blocking_bus.sync.wait(lock);
                   },
                   [=](const std::function<void(std::size_t)>& generator) { generator(quantity); },
               },
               device);
    return device_buf.rx.empty() ? 0 : device_buf.rx.front().size();
}

std::uint8_t TwoWire::requestFrom(std::uint8_t address, std::uint8_t size, std::uint8_t stop) {
    return requestFrom(address, static_cast<std::size_t>(size), static_cast<bool>(stop));
}

void TwoWire::beginTransmission(std::uint8_t address) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("Wire.beginTransmission({}): {}", +address, msg); };
    if (address & no_address)
        handle_error(debug_sig("Invalid address"));

    slave_address = address;
    write_buf_used = 0;
}

std::uint8_t TwoWire::endTransmission(std::uint8_t) {
    enum RetCode : std::uint8_t { success, truncated_data, nack_address [[maybe_unused]], nack_data [[maybe_unused]], other };
    if (!begun || slave_address == no_address)
        return RetCode::other;

    maybe_init();
    {
        std::lock_guard lock{board_data->i2c_buses[bus_id].devices_mut};
        auto& [device_buf, device] = board_data->i2c_buses[bus_id].slaves[slave_address];
        std::lock_guard tx_lock{device_buf.tx_mutex};
        auto& bus = board_data->i2c_buses[bus_id];
        auto& packet = device_buf.tx.emplace_back();
        packet.resize(write_buf_used);
        std::memcpy(packet.data(), write_buf.data(), write_buf_used);
    }

    write_buf_used = 0;
    if (truncated_buf) {
        truncated_buf = false;
        return RetCode::truncated_data;
    }
    return RetCode::success;
};

std::size_t TwoWire::write(std::uint8_t value) {
    if (!begun || slave_address == no_address)
        return 0;

    if (own_address != no_address) // CAVEAT: [missing] TWI slave mode is no-op
        return 0;

    if (write_buf_used + sizeof(value) >= write_buf.size())
        truncated_buf = true;

    if (write_buf_used == write_buf.size())
        return 0;

    write_buf[write_buf_used++] = static_cast<std::byte>(value);
    return sizeof(value);
}

std::size_t TwoWire::write(const std::uint8_t* data, std::size_t quantity) {
    if (!begun || !data || quantity == 0 || slave_address == no_address)
        return 0;

    if (own_address != no_address) // CAVEAT: [missing] TWI slave mode is no-op
        return 0;

    const auto fit = (std::min)(write_buf.size() - write_buf_used, quantity);
    std::memcpy(write_buf.data() + write_buf_used, data, fit);
    if (write_buf_used + quantity >= write_buf.size())
        truncated_buf = true;

    write_buf_used += fit;
    return fit;
}

int TwoWire::available() {
    if (!begun)
        return 0;
    std::lock_guard lock{board_data->i2c_buses[bus_id].devices_mut};
    auto& [device_buf, device] = board_data->i2c_buses[bus_id].slaves[slave_address];
    std::lock_guard rx_lock{device_buf.rx_mutex};
    return ranges::accumulate(device_buf.rx, 0, [](std::size_t acc, const auto& el) { return acc + el.size(); });
}

int TwoWire::peek() {
    if (!begun)
        return -1;
    std::lock_guard lock{board_data->i2c_buses[bus_id].devices_mut};
    auto& [device_buf, device] = board_data->i2c_buses[bus_id].slaves[slave_address];
    std::lock_guard rx_lock{device_buf.rx_mutex};
    if (device_buf.rx.empty())
        return -1;
    return static_cast<std::uint8_t>(device_buf.rx.front().front());
}

int TwoWire::read() {
    if (!begun)
        return -1;
    std::lock_guard lock{board_data->i2c_buses[bus_id].devices_mut};
    auto& [device_buf, device] = board_data->i2c_buses[bus_id].slaves[slave_address];
    std::lock_guard rx_lock{device_buf.rx_mutex};
    if (device_buf.rx.empty())
        return -1;
    const auto first = static_cast<std::uint8_t>(device_buf.rx.front().front());
    if(device_buf.rx.front().size() == 1)
        device_buf.rx.erase(device_buf.rx.begin());
    else
        device_buf.rx.front().erase(device_buf.rx.front().begin());
    return first;
}

void TwoWire::setClock(std::uint32_t) {}

void TwoWire::onReceive(OnRecieve* cb) noexcept { user_onrecieve = cb; } // CAVEAT: [missing] TWI slave mode is no-op

void TwoWire::onRequest(OnRequest* cb) noexcept { user_onrequest = cb; } // CAVEAT: [missing] TWI slave mode is no-op
