/*
 *  Server.hxx
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

#ifndef SMARTCAR_EMUL_SERVER_HXX
#define SMARTCAR_EMUL_SERVER_HXX

#include <optional>
#include <vector>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "Session.hxx"
#include "BluetoothDevice.hxx"

namespace smce::botcp {

class Server {
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::optional<Session> m_sess;
    std::vector<BluetoothDev>* m_devices_ptr;

    void do_accept() {
        m_acceptor.async_accept([this](boost::system::error_code ec, auto socket) {
            if (!ec && !m_sess)
                m_sess.emplace(std::move(socket), m_devices_ptr);
            do_accept();
        });
    }
  public:
    Server(boost::asio::io_context& io_context,
           const boost::asio::ip::tcp::endpoint& endpoint, std::vector<BluetoothDev>* devices_ptr)
        : m_acceptor{io_context, endpoint}, m_devices_ptr{devices_ptr} {
        do_accept();
    }


};

}

#endif // SMARTCAR_EMUL_SERVER_HXX
