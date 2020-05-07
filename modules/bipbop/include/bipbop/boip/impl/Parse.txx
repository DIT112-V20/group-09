/*
 *  Parse.txx
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

#ifndef SMARTCAR_EMUL_BOIP_PARSE_TXX
#define SMARTCAR_EMUL_BOIP_PARSE_TXX

#include <bit>
#include <cmath>
#include "../Address.hxx"
#include "../Message.hxx"
#include "../Parse.hxx"


namespace smce::boip {

namespace details {

enum HeaderByte : unsigned char {
    secure    = 0x80,
    bluetooth = 0x40,
    bluectl   = 0x20,
};

enum BluectlHeaderFields : unsigned char {
    auth  = 0x10,
    flags = 0x08,
};

enum BlueFlagsHeaderFields : unsigned char {
    master = 0x40
};

constexpr static bool is_secure(std::byte b) { return static_cast<unsigned char>(b) & secure; }
constexpr static bool is_plain(std::byte b) { return !is_secure(b); }
constexpr static bool is_bluetooth(std::byte b) { return static_cast<unsigned char>(b) & bluetooth; }
constexpr static bool is_hostctl(std::byte b) { return !is_bluetooth(b); }
constexpr static bool is_bluectl(std::byte b) { return static_cast<unsigned char>(b) & bluectl; }
constexpr static bool is_dataframe(std::byte b) { return !is_bluectl(b); }

constexpr static bool is_auth(std::byte b) { return static_cast<unsigned char>(b) & auth; }
constexpr static bool is_ctl(std::byte b) { return !is_auth(b); }
constexpr static bool is_masterrequest(std::byte b) { return static_cast<unsigned char>(b) & master; }
constexpr static bool is_slaverequest(std::byte b) { return !is_masterrequest(b); }

constexpr static bool is_pairing_request(std::byte b) { return (static_cast<unsigned char>(b) & 6) == 0; }
constexpr static bool is_pin_request(std::byte b) { return (static_cast<unsigned char>(b) & 2) == 2 && !is_pairing_request(b); }
constexpr static bool is_pin_providal(std::byte b) { return (static_cast<unsigned char>(b) & 4) == 4 && !is_pairing_request(b); }
constexpr static bool is_pairing_reply(std::byte b) { return (static_cast<unsigned char>(b) & 6) == 6; }


template <class F>
auto decrypt_and_call(F&& f, gsl::span<const std::byte> buffer, std::byte header)
    requires std::invoke<decltype(f), decltype(buffer)> {
    if(!is_secure(header))
        return std::forward<F>(f)(buffer);
    std::vector<std::byte> decrypted_buffer; // = decrypt_buffer(buffer);
    return std::forward<F>(f)(gsl::span<const std::byte>{decrypted_buffer});
}

template <class T>
T read_scalar(const std::byte* data_ptr) noexcept { // C++2a: make constexpr with std::bit_cast
    T ret;
    if constexpr (std::endian::native == std::endian::big) {
        std::copy(data_ptr, data_ptr + sizeof(T), static_cast<std::byte*>(&ret));
    } else {
        std::copy(std::make_reverse_iterator(data_ptr + sizeof(T)), std::make_reverse_iterator(data_ptr), reinterpret_cast<std::byte*>(&ret));
    }
    return ret;
}

}

template <class DataHandler, class PairingHandler, class RelationshipHandler>
std::streamsize parse_message(gsl::span<std::byte> buffer,
                              DataHandler&& data_handler,
                              PairingHandler&& pairing_handler,
                              RelationshipHandler&& rel_handler,
                              Endpoint& endpoint)
requires std::invocable<DataHandler, DataFrame>
         && std::invocable<PairingHandler, PairingFrame>
         && std::invocable<RelationshipHandler, bool> {
    if (buffer.empty())
        return 0;

    auto total_buffer_index = 0;
    const auto header = buffer[total_buffer_index++];
    if (details::is_bluetooth(header)) {
        if (details::is_dataframe(header)) {
            const auto payload_length = details::read_scalar<std::uint16_t>(&buffer[total_buffer_index]);
            total_buffer_index += sizeof(payload_length);
            const auto plain_read = details::decrypt_and_call([=, data_handler=std::forward<DataHandler>(data_handler)]
                                             (gsl::span<const std::byte> decrypted_buffer, bool was_encrypted) -> std::size_t {
                if(decrypted_buffer.size() < 2 * sizeof(Address) + (was_encrypted ? sizeof(std::uint16_t) : 0))
                    return -decrypted_buffer.size();
                auto buffer_index = 0;
                const auto real_payload_length = was_encrypted ? details::read_scalar<std::uint16_t>(&decrypted_buffer[buffer_index]) : payload_length;
                if (was_encrypted)
                    buffer_index += sizeof(real_payload_length);
                DataFrame frame;
                frame.receiver = details::read_scalar<Address>(&decrypted_buffer[buffer_index]);
                buffer_index += sizeof(Address);
                frame.receiver = details::read_scalar<Address>(&decrypted_buffer[buffer_index]);
                buffer_index += sizeof(Address);
                frame.payload = buffer.subspan(buffer_index, real_payload_length);
                if(decrypted_buffer.size() == buffer_index) {
                    /// TODO mark endpoint as disconnected
                    return buffer_index;
                }

                if (decrypted_buffer.size() < buffer_index + real_payload_length)
                    return -(buffer_index + real_payload_length);

                std::forward<DataHandler>(data_handler)(frame);
                return buffer_index + real_payload_length;
            }, buffer.subspan(total_buffer_index));
            return (total_buffer_index + (details::is_secure(header) ? payload_length : std::abs(plain_read))) * (plain_read >= 0 ? 1 : -1);
        }
        // bluectl
        if(details::is_auth(header)){
            /// TODO: Handle auth
            return total_buffer_index;
        }
        if(details::is_ctl(header)) {
            std::forward<RelationshipHandler>(rel_handler)(details::is_masterrequest(header));
            return total_buffer_index;
        }
        // Pairing
        if(buffer.size() < (total_buffer_index + sizeof(Address) * 2))
            return -total_buffer_index;
        PairingFrame pframe;
        pframe.status = PairingFrame::Status(static_cast<std::uint8_t>(header >> 1) & 3u);
        pframe.receiver = details::read_scalar<Address>(&buffer[total_buffer_index]);
        total_buffer_index += sizeof(Address);
        pframe.receiver = details::read_scalar<Address>(&buffer[total_buffer_index]);
        total_buffer_index += sizeof(Address);

        if(details::is_pairing_request(header)){
            if(buffer.size() < (total_buffer_index + sizeof(std::uint8_t)))
                return -total_buffer_index;
            const auto name_len = details::read_scalar<std::uint8_t>(&buffer[total_buffer_index++]);
            if(buffer.size() < (total_buffer_index + name_len))
                return -total_buffer_index;
            pframe.sender_name = gsl::span<const char>{reinterpret_cast<const char*>(&buffer[total_buffer_index]), name_len};
            total_buffer_index += name_len;
        } else if(details::is_pin_providal(header)){
            if(buffer.size() < (total_buffer_index + sizeof(std::uint64_t)))
                return -total_buffer_index;
            pframe.pin = details::read_scalar<std::uint64_t>(&buffer[total_buffer_index]);
            total_buffer_index += sizeof(std::uint64_t);
        }
        std::forward<PairingHandler>(pairing_handler)(pframe);
        return total_buffer_index;
    }
    return -total_buffer_index; // Fail so long RCSMCE is not implemented
}

}

#endif // SMARTCAR_EMUL_BOIP_PARSE_TXX