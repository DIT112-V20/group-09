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
    "required": [
        "name",
        "fqbn",
        "pin_count",
        "digital_in_unable_pins",
        "digital_out_unable_pins",
        "analog_in_capable_pins",
        "analog_out_capable_pins",
        "interruptable_pins",
        "pwm_capable_pins",
        "uart_quads",
        "i2c_pairs"
    ],
    "properties": {
        "name": { "type": "string" },
        "fqbn": { "type": "string" },
        "pin_count": { "type": "integer" },
        "analog_in_capable_pins": {
            "type": "array",
            "items": { "type": "integer" }
        },
        "analog_out_capable_pins": {
            "type": "array",
            "items": { "type": "integer" }
        },
        "digital_in_unable_pins": {
            "type": "array",
            "items": { "type": "integer" }
        },
        "digital_out_unable_pins": {
            "type": "array",
            "items": { "type": "integer" }
        },
        "interruptable_pins": {
            "type": "array",
            "items": { "type": "integer" }
        },
        "pwm_capable_pins": {
            "type": "array",
            "items": { "type": "integer" }
        },
        "uart_quads": {
            "type": "array",
            "items": {
                "type": "object",
                "required": [
                    "rx_pin",
                    "tx_pin",
                    "cts_pin",
                    "rts_pin"
                ],
                "properties": {
                    "rx_pin": { "type": "integer" },
                    "tx_pin": { "type": "integer" },
                    "cts_pin": { "type": "integer" },
                    "rts_pin": { "type": "integer" }
                }
            }
        },
        "i2c_pairs": {
            "type": "array",
            "items": {
                "type": "object",
                "required": [
                    "sda_pin",
                    "scl_pin"
                ],
                "properties": {
                    "sda_pin": { "type": "integer" },
                    "scl_pin": { "type": "integer" }
                }
            }
        },
        "spi_quads": {
            "type": "array",
            "items": {
                "type": "object",
                "required": [
                    "mosi_pin",
                    "miso_pin",
                    "clk_pin",
                    "cs_pin"
                ],
                "properties": {
                    "mosi_pin": { "type": "integer" },
                    "miso_pin": { "type": "integer" },
                    "clk_pin": { "type": "integer" },
                    "cs_pin": { "type": "integer" }
                }
            }
        }
    }
}
)";

}
