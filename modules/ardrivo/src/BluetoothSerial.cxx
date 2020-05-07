/*
 *  BluetoothSerial.cxx
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

#include <cstring>
#include "BoardData.hxx"
#include "BoardDataDef.hxx"
#include "BluetoothSerial.h"
#include "Entrypoint.hxx"

BluetoothSerial::BluetoothSerial() {}

bool BluetoothSerial::begin(String local_name, bool is_master) {
    if(m_begun)
        return false;
    m_begun = true;
    m_local_name = {local_name.c_str(), local_name.length()};
    maybe_init();
    board_data->bluedevs[m_dev_id].master = is_master;
    return true;
}

int BluetoothSerial::available() { return m_rx_buf.size(); }

int BluetoothSerial::peek()  { return m_rx_buf.empty() ? -1 : m_rx_buf.front(); }

bool BluetoothSerial::hasClient() const {
    return static_cast<bool>(m_remote_address) && (maybe_init(), board_data->bluedevs[m_dev_id].master);
}

int BluetoothSerial::read()  {
    const auto first = peek();
    if(first >= 0)
        m_rx_buf.erase(m_rx_buf.begin());
    return first;
}
std::size_t BluetoothSerial::write(std::uint8_t c) {}

std::size_t BluetoothSerial::write(const std::uint8_t* buffer, std::size_t size) {}

void BluetoothSerial::flush() {}

void BluetoothSerial::end() {
    disconnect();
    m_begun = false;
}

void BluetoothSerial::onData(BluetoothSerialDataCb cb) {
    maybe_init();
    board_data->bluedevs[m_dev_id].on_data = [&, cb=std::move(cb)](smce::boip::DataFrame df) {
        if(df.sender != *this->m_remote_address)
            return;
        cb(reinterpret_cast<const std::uint8_t*>(df.payload.data()), df.payload.size());
    };
}

//esp_err_t BluetoothSerial::register_callback(esp_spp_cb_t * callback){}

void BluetoothSerial::enableSSP() {}

bool BluetoothSerial::setPin(const char* pin) {
    const auto s_len = std::strlen(pin);
    if(s_len > sizeof(m_pin))
        return false;
    m_pin = 0;
    std::memcpy(&m_pin, pin, s_len);
    return true;
}

bool BluetoothSerial::connect(String remote_name) { return false; } // CAVEAT: BluetoothSerial: BoIP does not support scanning yet

bool BluetoothSerial::connect(std::array<std::byte, 6> remote_address) {
    if(!m_begun)
        return false;
    if(!board_data->bluetooth_send_pairing)
        return false;
    (*board_data->bluetooth_send_pairing)({
                                              .status = smce::boip::PairingFrame::Status::pairing_request,
                                              .sender = board_data->bluedevs[m_dev_id].address,
                                              .receiver = *m_remote_address,
                                              .sender_name = m_local_name,
                                          });
    std::condition_variable cv;
    board_data->bluedevs[m_dev_id].on_pairing = [&](smce::boip::PairingFrame frame) {

    };
}

bool BluetoothSerial::connect(const std::uint8_t* remote_address) {
    std::memcpy(m_remote_address->data(), remote_address, m_remote_address->size());
    return connect();
}

bool BluetoothSerial::connect() { return connect(*m_remote_address); }
bool BluetoothSerial::connected(int) {
    return m_remote_address.has_value();
}
bool BluetoothSerial::isReady(bool check_master, int timeout) const {}

bool BluetoothSerial::disconnect() { m_remote_address = std::nullopt; }

bool BluetoothSerial::unpairDevice(const std::uint8_t* remote_address) {}