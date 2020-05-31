/*
 *  Servo.cxx
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

#include <fmt/format.h>
#include "Arduino.h"
#include "BoardDataDef.hxx"
#include "Servo.h"
#include "Entrypoint.hxx"
#include "Error.hxx"

constexpr int servo_min(int8_t min) { return MIN_PULSE_WIDTH - min * 4; }
constexpr int servo_max(int8_t max) { return MAX_PULSE_WIDTH - max * 4; }
constexpr uint8_t TRIM_DURATION = 2;

void Servo::attach(int pin) { this->attach(pin, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH); }

void Servo::attach(int pin, int minWidth, int maxWidth) {
    maybe_init();
    const auto debug_sig = [=](const char* msg) { return fmt::format("attach({}, {}, {}): {}", pin, minWidth, maxWidth, msg); };
    if (pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"));
    if (!board_info->pins_caps[pin].pwm_able)
        return handle_error(debug_sig("Pin does not support PWM"));

    attached_pin = pin;
    min = (MIN_PULSE_WIDTH - minWidth) / 4;
    max = (MAX_PULSE_WIDTH - maxWidth) / 4;
}

void Servo::detach() { attached_pin = -1; }

void Servo::write(int value) {
    maybe_init();
    if (attached())
        board_data->pwm_values[attached_pin] = std::clamp(value, 0, 180);
}

void Servo::writeMicroseconds(int value) {
    maybe_init();
    if (attached())
        this->write(map(std::clamp(value, servo_min(min), servo_max(max)) - TRIM_DURATION, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, 0,
                        180)); // Convert microseconds to degree
}

int Servo::read() {
    maybe_init();
    return attached() ? board_data->pwm_values[attached_pin].load() : -1;
}

bool Servo::attached() const noexcept { return attached_pin != -1; }