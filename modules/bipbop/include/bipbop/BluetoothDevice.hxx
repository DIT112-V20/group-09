/*
 *  BluetoothDevice.hxx
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

#ifndef SMARTCAR_EMUL_DEVICE_HXX
#define SMARTCAR_EMUL_DEVICE_HXX

#include "boip/Address.hxx"
#include "boip/Message.hxx"

using BluetoothSendData = std::function<bool(smce::boip::DataFrame)>;
using BluetoothSendPairing = std::function<bool(smce::boip::PairingFrame)>;
struct BluetoothDev {
    smce::boip::Address address{};
    bool master = false;
    bool connected = false;
    std::function<void(smce::boip::DataFrame)> on_data;
    std::function<void(smce::boip::PairingFrame)> on_pairing;
};

#endif // SMARTCAR_EMUL_DEVICE_HXX
