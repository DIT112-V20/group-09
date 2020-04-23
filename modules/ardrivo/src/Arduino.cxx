/*
 *  Arduino.cxx
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

#include <climits>
#include <string>
#include <thread>
#include <fmt/format.h>
#include "Arduino.h"
#include "BoardData.hxx"
#include "BoardDataDef.hxx"
#include "BoardInfo.hxx"
#include "Error.hxx"

using namespace std::literals;

void pinMode(uint8_t pin, uint8_t mode) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("pinMode({}, {}): {}", pin, mode, msg); };
    if (pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"));
    if ((mode == INPUT || mode == INPUT_PULLUP) && !(board_info->pins_caps[pin].digital_in || board_info->pins_caps[pin].analog_in))
        return handle_error(debug_sig("Pin cannot be used for digital input"));
    if (mode == OUTPUT  && !(board_info->pins_caps[pin].digital_out || board_info->pins_caps[pin].analog_out))
        return handle_error(debug_sig("Pin cannot be used for digital output"));
    board_data->pin_modes[pin] = mode;
}

void digitalWrite(uint8_t pin, bool val) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("digitalWrite({}, {}): {}", pin, val, msg); };
    if (pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"));
    if (!board_info->pins_caps[pin].digital_out)
        return handle_error(debug_sig("Pin cannot be used for digital output"));
    board_data->digital_pin_values[pin] = val;
}

int digitalRead(uint8_t pin) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("digitalRead({}): {}", pin, msg); };
    if (pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"), LOW);
    if (!board_info->pins_caps[pin].digital_in)
        return handle_error(debug_sig("Pin cannot be used for digital input"), LOW);
    return board_data->digital_pin_values[pin];
}

void analogWrite(uint16_t pin, uint16_t val) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("analogWrite({}, {}): {}", pin, val, msg); };
    if (pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"));
    if (!board_info->pins_caps[pin].analog_out)
        return handle_error(debug_sig("Pin cannot be used for analog output"));
    board_data->analog_pin_values[pin] = val;
}

int analogRead(uint8_t pin) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("analogRead({}): {}", pin, msg); };
    if (pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"), 0);
    if (!board_info->pins_caps[pin].analog_in)
        return handle_error(debug_sig("Pin cannot be used for analog input"), 0);
    return board_data->analog_pin_values[pin];
}

void analogReference(uint8_t mode) {}

void noTone(uint8_t pin) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("noTone({}): {}", pin, msg); };
    if (pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"));
    constexpr uint8_t noToneFeq = 255;
    board_data->pin_frequency[pin] = noToneFeq;
}

void tone(uint8_t pin, unsigned int frequency, unsigned long duration) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("tone({}, {}, {}): {}", pin, frequency, duration, msg); };
    if (pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"));
    board_data->pin_frequency[pin] = frequency;
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    noTone(pin);
}

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("pulseIn({}, {}, {}): {}", pin, state, timeout, msg); };
    if (pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"), 0);
    const std::atomic_bool& pin_state = board_data->digital_pin_values[pin];

    const auto wait_state_change = [&](bool target, decltype(timeout) timeout = 0) {
      auto t_max = std::chrono::steady_clock::now() + std::chrono::microseconds(timeout);
      while (HIGH) {
          const auto t = std::chrono::steady_clock::now();
          if (pin_state == target)
              return LOW;
          if (timeout > 0 && t >= t_max)
              return HIGH;
          std::this_thread::sleep_for(1ms);
      }
    };

    if (pin_state == state) {
        if (wait_state_change(!state, timeout)) // timeout reached
            return 0;
    }

    const auto start_time = std::chrono::steady_clock::now();
    wait_state_change(state);
    const auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start_time).count();
}

unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("pulseInLong({}, {}, {}): {}", pin, state, timeout, msg); };
    if (pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"), 0);
    return pulseIn(pin, state, timeout);
}

void shiftOut(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order, uint8_t val) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("shiftOut({}, {}, {}, {}): {}", data_pin, clock_pin, bit_order, val, msg); };
    if (data_pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"));

    for (uint8_t i = 0; i < CHAR_BIT; ++i) {
        if (bit_order == RIGHTMOSTBIT) {
            digitalWrite(data_pin, val & 1u);
            val >>= 1u;
        } else {
            digitalWrite(data_pin, (val & (1u << static_cast<unsigned>(CHAR_BIT - 1))) != 0);
            val <<= 1u;
        }

        digitalWrite(clock_pin, HIGH);
        digitalWrite(clock_pin, LOW);
    }
}
uint8_t shiftIn(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("shiftIn({}, {}, {}): {}", data_pin, clock_pin, bit_order, msg); };
    if (data_pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"), '\x00');
    
    uint8_t value = 0;
    for (uint8_t i = 0; i < CHAR_BIT; ++i) {
        digitalWrite(clock_pin, '\x01');
        value |= static_cast<byte>(digitalRead(data_pin)) << static_cast<unsigned>(bit_order == RIGHTMOSTBIT ? i : (CHAR_BIT - 1) - i);
        digitalWrite(clock_pin, LOW);
    }
    return value;
}

void delay(unsigned long duration) { std::this_thread::sleep_for(std::chrono::milliseconds(duration)); }

void delayMicroseconds(unsigned int duration) { std::this_thread::sleep_for(std::chrono::microseconds(duration)); }

unsigned long micros() {
    const auto current_time = std::chrono::steady_clock::now();
    const unsigned long duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time - board_data->start_time).count();
    return duration;
}

unsigned long millis() {
    const auto current_time = std::chrono::steady_clock::now();
    const unsigned long duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - board_data->start_time).count();
    return duration;
}


// Arduino Math functions
long constrain(long amt, long low, long high) { return (amt < low ? low : (amt > high ? high : amt)); }
long map(long x, long in_min, long in_max, long out_min, long out_max) { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }

// Arduino Characters functions
bool isAlpha(char thisChar) { return std::isalpha(thisChar); }
bool isAlphaNumeric(char thisChar) { return std::isalnum(thisChar); }
bool isAscii(char thisChar) { return +thisChar < +'\x80'; }
bool isControl(char thisChar) { return std::iscntrl(thisChar); }
bool isDigit(char thisChar) { return std::isdigit(thisChar); }
bool isGraph(char thisChar) { return std::isgraph(thisChar); }
bool isHexadecimalDigit(char thisChar) { return std::isxdigit(thisChar); };
bool isLowerCase(char thisChar) { return std::islower(thisChar); }
bool isPrintable(char thisChar) { return std::isprint(thisChar); }
bool isPunct(char thisChar) { return std::ispunct(thisChar); }
bool isSpace(char thisChar) { return std::isspace(thisChar); }
bool isUpperCase(char thisChar) { return std::isupper(thisChar); }
bool isWhitespace(char thisChar) { return std::isblank(thisChar); }

// Arduino Random Numbers functions
long random(long max) { return std::rand() % max; }
long random(long min, long max) { return std::rand() % (max - min) + min; }
void randomSeed(unsigned long seed) { return std::srand(seed); }

// Arduino External Interrupts functions
void attachInterrupt(uint8_t pin, void (*user_func)(), int mode) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("attachInterrupt({}, /* function pointer */, {}): {}", pin, mode, msg); };
    if (pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"));
    if (!board_info->pins_caps[pin].interrupt_able)
        return handle_error(debug_sig("Pin cannot have interrupts attached to it"));
    
    if (pin < board_data->interrupts_handlers.size())
        board_data->interrupts_handlers[pin] = std::pair{user_func, mode};
}
void detachInterrupt(uint8_t pin) {
    const auto debug_sig = [=](const char* msg) { return fmt::format("detachInterrupt({}): {}", pin, msg); };
    if (pin >= board_info->pins_caps.size())
        return handle_error(debug_sig("Pin does not exist"));
    if (!board_info->pins_caps[pin].interrupt_able)
        return handle_error(debug_sig("Pin cannot have interrupts attached to it"));
    if (pin < board_data->interrupts_handlers.size())
        board_data->interrupts_handlers.erase(board_data->interrupts_handlers.begin() + pin);
}