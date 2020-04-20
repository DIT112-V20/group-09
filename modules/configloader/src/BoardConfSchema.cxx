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
    "$schema": "http://json-schema.org/draft-07/schema",
    "$id": "http://example.com/example.json",
    "type": "object",
    "title": "The Root Schema",
    "description": "The root schema comprises the entire JSON document.",
    "default": {},
    "additionalProperties": true,
    "required": [
        "name",
        "fqbn",
        "digital_pin_count",
        "analog_pin_count",
        "interruptable_digital_pins",
        "pwm_capable_pins",
        "uart_pairs"
    ],
    "properties": {
        "name": {
            "$id": "#/properties/name",
            "type": "string",
            "title": "The Name Schema",
            "description": "Name of this board configuration",
            "default": "",
            "examples": [
                "esp32-dev"
            ]
        },
        "fqbn": {
            "$id": "#/properties/fqbn",
            "type": "string",
            "title": "The Fqbn Schema",
            "description": "Fully Qualified Board Name as known to ArduinoCLI",
            "default": "",
            "examples": [
                "esp32:esp32:esp32"
            ]
        },
        "digital_pin_count": {
            "$id": "#/properties/digital_pin_count",
            "type": "integer",
            "title": "The Digital_pin_count Schema",
            "description": "Number of digital pins",
            "default": 0,
            "examples": [
                36.0
            ]
        },
        "analog_pin_count": {
            "$id": "#/properties/analog_pin_count",
            "type": "integer",
            "title": "The Analog_pin_count Schema",
            "description": "Number of analog pins.",
            "default": 0,
            "examples": [
                18.0
            ]
        },
        "interruptable_digital_pins": {
            "$id": "#/properties/interruptable_digital_pins",
            "type": "array",
            "title": "The Interruptable_digital_pins Schema",
            "description": "Which pins can have interrupts attached to them.",
            "default": [],
            "examples": [
                [
                    5.0,
                    9.0
                ]
            ],
            "additionalItems": true,
            "items": {
                "$id": "#/properties/interruptable_digital_pins/items",
                "type": "integer",
                "title": "The Items Schema",
                "description": "The GPIO pin number.",
                "default": 0,
                "examples": [
                    5.0,
                    9.0
                ]
            }
        },
        "pwm_capable_pins": {
            "$id": "#/properties/pwm_capable_pins",
            "type": "array",
            "title": "The Pwm_capable_pins Schema",
            "description": "Which analog pins are PWM-capable.",
            "default": [],
            "examples": [
                [
                    1.0,
                    2.0,
                    3.0
                ]
            ],
            "additionalItems": true,
            "items": {
                "$id": "#/properties/pwm_capable_pins/items",
                "type": "integer",
                "title": "The Items Schema",
                "description": "The GPIO pin number.",
                "default": 0,
                "examples": [
                    1.0,
                    2.0,
                    3.0
                ]
            }
        },
        "uart_pairs": {
            "$id": "#/properties/uart_pairs",
            "type": "array",
            "title": "The Uart_pairs Schema",
            "description": "An explanation about the purpose of this instance.",
            "default": [],
            "examples": [
                [
                    {
                        "rx_pin": 3.0,
                        "tx_pin": 1.0
                    },
                    {
                        "rx_pin": 16.0,
                        "tx_pin": 17.0
                    }
                ]
            ],
            "additionalItems": true,
            "items": {
                "$id": "#/properties/uart_pairs/items",
                "type": "object",
                "title": "The Items Schema",
                "description": "An explanation about the purpose of this instance.",
                "default": {},
                "examples": [
                    {
                        "rx_pin": 3.0,
                        "tx_pin": 1.0
                    },
                    {
                        "rx_pin": 16.0,
                        "tx_pin": 17.0
                    }
                ],
                "additionalProperties": true,
                "required": [
                    "rx_pin",
                    "tx_pin"
                ],
                "properties": {
                    "rx_pin": {
                        "$id": "#/properties/uart_pairs/items/properties/rx_pin",
                        "type": "integer",
                        "title": "The Rx_pin Schema",
                        "description": "An explanation about the purpose of this instance.",
                        "default": 0,
                        "examples": [
                            3.0
                        ]
                    },
                    "tx_pin": {
                        "$id": "#/properties/uart_pairs/items/properties/tx_pin",
                        "type": "integer",
                        "title": "The Tx_pin Schema",
                        "description": "An explanation about the purpose of this instance.",
                        "default": 0,
                        "examples": [
                            1.0
                        ]
                    }
                }
            }
        }
    }
}
)";

}
