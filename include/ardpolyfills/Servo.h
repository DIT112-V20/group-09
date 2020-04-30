/*
 *  Servo.h
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

#ifndef Servo_h
#define Servo_h

#include <cstdint>

enum SMCE__Boundary{ MIN_PULSE_WIDTH = 1000, MAX_PULSE_WIDTH = 2000, DEFAULT_PULSE_WIDTH = 1500 };

class Servo {
  public:
    constexpr Servo() = default;
    void attach(int pin);
    void attach(int pin, int min, int max);
    void detach();
    void write(int value);
    void writeMicroseconds(int value);
    int read();
    bool attached() const noexcept;

  private:
    int attached_pin = -1;
    int8_t min = 0;
    int8_t max = 0;
};

#endif // Servo_h
