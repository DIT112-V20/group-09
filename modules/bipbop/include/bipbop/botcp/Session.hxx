/*
 *  Session.hxx
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

#ifndef SMARTCAR_EMUL_SESSION_HXX
#define SMARTCAR_EMUL_SESSION_HXX

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include "BluetoothDevice.hxx"
#include "boip/Endpoint.hxx"
#include "boip/Parse.hxx"

namespace smce::botcp {

class Session {
    boost::asio::ip::tcp::socket m_socket;
    boip::Endpoint m_endpoint{};
    std::vector<BluetoothDev>* m_devices_ptr;
    char m_data[1024];

    BluetoothDev* get_device(boip::Address address) {
        if (!m_devices_ptr)
            return nullptr;
        for (auto& dev : *m_devices_ptr) {
            if (dev.address == address)
                return &dev;
        }
        return nullptr;
    }

    void do_read() {
        boost::asio::async_read(m_socket,
                                boost::asio::buffer(m_data),
                                [this](boost::system::error_code ec, std::size_t length) {
                                  if (ec)
                                    return do_read();
                                  boip::parse_message(gsl::span<const std::byte>{reinterpret_cast<const std::byte*>(+m_data), length},
                                      [&](boip::DataFrame frame){
                                        if (auto* const dev = get_device(frame.receiver); dev) {
                                            if(dev->on_data)
                                                dev->on_data(frame);
                                        }
                                  }, [&](boip::PairingFrame frame) {
                                        if (auto* const dev = get_device(frame.receiver); dev) {
                                          if(dev->on_pairing)
                                              dev->on_pairing(frame);
                                      }
                                  }, [](bool){/* ignore */}, m_endpoint);
                                  do_read();
                                });
    }
  public:
    explicit Session(boost::asio::ip::tcp::socket socket, std::vector<BluetoothDev>* devices_ptr)
        : m_socket{std::move(socket)}, m_devices_ptr{devices_ptr} {
        do_read();
    }
};

}

#endif // SMARTCAR_EMUL_SESSION_HXX
