/*
 *  BluetoothSerial.h
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

#ifndef BluetoothSerial_h
#define BluetoothSerial_h

#include <array>
#include <cstddef>
#include <functional>
#include <string>
#include <vector>
#include "Stream.h"
#include "WString.h"

using BluetoothSerialDataCb = std::function<void(const std::uint8_t* buffer, std::size_t size)>;

class BluetoothSerial : public Stream {
    std::string m_local_name;
    std::uint64_t m_pin;
    std::uint8_t m_dev_id{};
    std::vector<std::uint8_t> m_rx_buf;
    std::vector<std::uint8_t> m_tx_buf;
    std::optional<std::array<std::byte, 6>> m_remote_address;
    bool m_begun = false;

    bool connect(std::array<std::byte, 6> remote_address);
  public:
    BluetoothSerial();

    bool begin(String local_name = {}, bool is_master = false);
    int available() override;
    int peek() override;
    bool hasClient() const;
    int read() override;
    std::size_t write(std::uint8_t c) override;
    std::size_t write(const std::uint8_t* buffer, std::size_t size) override;
    void flush() override;
    void end();
    void onData(BluetoothSerialDataCb cb);
    //esp_err_t register_callback(esp_spp_cb_t * callback);

    void enableSSP();
    bool setPin(const char* pin);
    bool connect(String remote_name);
    bool connect(const std::uint8_t* remote_address);
    bool connect();
    bool connected(int timeout = 0);
    bool isReady(bool check_master = false, int timeout = 0) const;
    bool disconnect();
    bool unpairDevice(const std::uint8_t* remote_address);
};

#endif // BluetoothSerial_h
