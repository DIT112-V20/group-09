/*
 *  BoardConfSchema.cxx
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

namespace smce {

const char* board_conf_schema = R"(
{
    "type": "object",
    "properties": {
        "name": { "type": "string" },
        "fqbn": { "type": "string" },
        "digital_pin_count": { "type": "integer" },
        "analog_pin_count": { "type": "integer" },
        "interruptable_digital_pins": {
            "type": "array",
            "items": { "type": "integer" }
        },
        "pwm_capable_pins": {
            "type": "array",
            "items": { "type": "integer" }
        },
        "uart_pairs": {
            "type": "array",
            "items": {
                "type": "object",
                "required": [
                    "rx_pin",
                    "tx_pin"
                ],
                "properties": {
                    "rx_pin": { "type": "integer" },
                    "tx_pin": { "type": "integer" }
                }
            }
        }
    }
}
)";

}
