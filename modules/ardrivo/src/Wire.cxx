#include <cstring>
#include <fmt/format.h>
#include <range/v3/algorithm/find_if.hpp>
#include "BoardDataDef.hxx"
#include "Entrypoint.hxx"
#include "Error.hxx"
#include "Wire.h"
#include "utility.hxx"

void TwoWire::begin() {
    begun = true;
}

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

    const auto it = ranges::find_if(board_info->i2c_chans, [=](const I2cChannelInfo& chan){
        return std::pair{+chan.sda_pin, +chan.scl_pin} == std::pair{sda, scl};
    });
    if(it == board_info->i2c_chans.end())
        return handle_error("SDA SCL pair are not an I2C channel");

    begin();
    bus_id = std::distance(board_info->i2c_chans.begin(), it);
}
void TwoWire::begin(int sda, int scl, std::uint8_t with_address) {
    maybe_init();
    const auto debug_sig = [=](const char* msg) { return fmt::format("Wire.begin({}, {}, {}): {}", sda, scl, +with_address, msg); };
    if(with_address & no_address)
        handle_error(debug_sig("Invalid address"));
    begin(sda, scl);
    begin(with_address);
}

std::size_t TwoWire::requestFrom(std::uint8_t address, std::size_t quantity, [[maybe_unused]] bool stop) {
    if(!begun || quantity == 0)
        return 0;
    maybe_init();
    const auto debug_sig = [=](const char* msg) { return fmt::format("Wire.requestFrom({}, {}, {}): {}", +address, quantity, stop, msg); };
    if(address & no_address)
        handle_error(debug_sig("Invalid address"));

    std::unique_lock lock{board_data->i2c_buses[bus_id].devices_mut};
    auto& device = board_data->i2c_buses[bus_id].devices[address];

    if (device.valueless_by_exception()) // not initialized -- dynamic init
        return 0; // CAVEAT: [dynamic-init] TWI IO is a no-op at dynamic init

    lock = std::visit(Visitor{
                   [quantity, lock = std::move(lock)](std::pair<std::condition_variable, std::size_t>& cv_and_len) mutable {
                        auto& [cv, size] = cv_and_len;
                       size = quantity;
                       cv.wait(lock);
                       return std::move(lock);
                   },
                   [=, lock = std::move(lock)](const std::function<void(std::size_t)>& generator) mutable { generator(quantity); return std::move(lock); },
               }, device);
    return board_data->i2c_buses[bus_id].rx_used_size;
}

std::uint8_t TwoWire::requestFrom(std::uint8_t address, std::uint8_t size, std::uint8_t stop) {
    return requestFrom(address, static_cast<std::size_t>(size), static_cast<bool>(stop));
}

uint8_t TwoWire::requestFrom(int address, int size, int stop) {
    return requestFrom(static_cast<std::uint8_t>(address), static_cast<std::size_t>(size),static_cast<bool>(stop));
}

void TwoWire::beginTransmission(std::uint8_t address) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("Wire.beginTransmission({}): {}", +address, msg); };
    if(address & no_address)
        handle_error(debug_sig("Invalid address"));

    slave_address = address;
    write_buf_used = 0;
}

std::uint8_t TwoWire::endTransmission(std::uint8_t) {
    enum RetCode : std::uint8_t {
        success,
        truncated_data,
        nack_address [[maybe_unused]],
        nack_data [[maybe_unused]],
        other
    };
    if (!begun || slave_address == no_address)
        return RetCode::other;

    maybe_init();
    {
        std::lock_guard lock{board_data->i2c_buses[bus_id].tx_mutex};
        auto& bus = board_data->i2c_buses[bus_id];
        bus.tx_used_size = write_buf_used;
        std::memcpy(bus.tx.data(), write_buf.data(), write_buf.size());
    }

    slave_address = no_address;
    if (truncated_buf) {
        truncated_buf = false;
        return RetCode::truncated_data;
    }
    return RetCode::success;
};

std::size_t TwoWire::write(std::uint8_t value) {
    if(!begun || slave_address == no_address)
        return 0;

    if(own_address != no_address) // CAVEAT: [missing] TWI slave mode is no-op
        return 0;

    if(write_buf_used + sizeof(value) >= write_buf.size())
        truncated_buf = true;

    if(write_buf_used == write_buf.size())
        return 0;

    write_buf[write_buf_used++] = static_cast<std::byte>(value);
    return sizeof(value);
}

std::size_t TwoWire::write(const std::uint8_t* data, std::size_t quantity) {
    if(!begun || !data || quantity == 0 || slave_address == no_address)
        return 0;

    if(own_address != no_address) // CAVEAT: [missing] TWI slave mode is no-op
        return 0;

    const auto fit = (std::min)(write_buf.size() - write_buf_used, quantity);
    std::memcpy(write_buf.data() + write_buf_used, data, fit);
    if (write_buf_used + quantity >= write_buf.size())
        truncated_buf = true;

    write_buf_used += fit;
    return fit;
}

int TwoWire::available() {
    if(!begun)
        return 0;
    std::lock_guard lock{board_data->i2c_buses[bus_id].rx_mutex};
    return board_data->i2c_buses[bus_id].rx_used_size;
}

int TwoWire::peek() {
    if(!begun)
        return -1;
    std::lock_guard lock{board_data->i2c_buses[bus_id].rx_mutex};
    if(board_data->i2c_buses[bus_id].rx_used_size == 0)
        return -1;
    return static_cast<std::uint8_t>(board_data->i2c_buses[bus_id].rx.front());
}

int TwoWire::read() {
    if(!begun)
        return -1;
    std::lock_guard lock{board_data->i2c_buses[bus_id].rx_mutex};
    if(board_data->i2c_buses[bus_id].rx_used_size == 0)
        return -1;
    const auto first = static_cast<std::uint8_t>(board_data->i2c_buses[bus_id].rx.front());
    std::memmove(board_data->i2c_buses[bus_id].rx.data(), board_data->i2c_buses[bus_id].rx.data() + 1, --board_data->i2c_buses[bus_id].rx_used_size);
    return first;
}

void TwoWire::setClock(std::uint32_t) {}

void TwoWire::onReceive(OnRecieve* cb) noexcept { user_onrecieve = cb; } // CAVEAT: [missing] TWI slave mode is no-op

void TwoWire::onRequest(OnRequest* cb) noexcept { user_onrequest = cb; }  // CAVEAT: [missing] TWI slave mode is no-op
