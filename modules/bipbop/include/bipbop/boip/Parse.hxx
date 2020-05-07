/*
 *  Parse.hxx
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

#ifndef SMARTCAR_EMUL_BOIP_PARSE_HXX
#define SMARTCAR_EMUL_BOIP_PARSE_HXX

#include <concepts>
#include <cstdint>
#include <gsl/gsl>

namespace smce::boip {

struct Endpoint;
struct DataFrame;
struct PairingFrame;

/**
 * Parses a message from the buffer
 * \param buffer from which the message will be read
 * \param message which will be populated
 * \param endpoint to use as context
 * \return the number of bytes which have been read from the buffer; negative on error, including zero
 **/
template <class DataHandler, class PairingHandler, class RelationshipHandler>
std::streamsize parse_message(gsl::span<std::byte> buffer,
                              DataHandler&& data_handler,
                              PairingHandler&& pairing_handler,
                              RelationshipHandler&& rel_handler,
                              Endpoint& endpoint)
    requires std::invocable<DataHandler, DataFrame>
        && std::invocable<PairingHandler, PairingFrame>
        && std::invocable<RelationshipHandler, bool>;


}

#include "impl/Parse.txx"

#endif // SMARTCAR_EMUL_BOIP_PARSE_HXX
