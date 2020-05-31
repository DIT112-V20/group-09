/*
 *  Arduino.h
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

#ifndef Arduino_h
#define Arduino_h
#include <cctype>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include "SMCE__dll.hxx"

#define digitalPinToInterrupt(x) (x)

enum SignificantBit : bool { RIGHTMOSTBIT, LEFTMOSTBIT };

#ifdef SMCE__COMPILING_ARDUINO_CXX
#   define SMCE__PINMODE_ENUM_STRUCT struct
#else
#   define SMCE__PINMODE_ENUM_STRUCT
#endif

enum SMCE__PINMODE_ENUM_STRUCT PinMode : std::uint8_t { INPUT, OUTPUT, INPUT_PULLUP };

enum DigitalValue : bool { LOW, HIGH };

// Digital and analog i/o
SMCE__DLL_RT_API void pinMode(uint8_t pin, uint8_t mode);
SMCE__DLL_RT_API void digitalWrite(uint8_t pin, bool val);
SMCE__DLL_RT_API int digitalRead(uint8_t pin);
SMCE__DLL_RT_API int analogRead(uint8_t pin);
SMCE__DLL_RT_API void analogReference(uint8_t mode);
SMCE__DLL_RT_API void analogWrite(uint16_t pin, uint16_t val);

// advanced i/o
SMCE__DLL_RT_API void tone(uint8_t pin, unsigned int frequency, unsigned long duration);
SMCE__DLL_RT_API void noTone(uint8_t _pin);
SMCE__DLL_RT_API unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
SMCE__DLL_RT_API unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout);
SMCE__DLL_RT_API void shiftOut(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order, uint8_t val);
SMCE__DLL_RT_API uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

// time
SMCE__DLL_RT_API void delay(unsigned long duration);
SMCE__DLL_RT_API void delayMicroseconds(unsigned int duration);
SMCE__DLL_RT_API unsigned long micros();
SMCE__DLL_RT_API unsigned long millis();

// Arduino Math functions
using std::abs;
SMCE__DLL_RT_API long constrain(long amt, long low, long high);
SMCE__DLL_RT_API long map(long x, long in_min, long in_max, long out_min, long out_max);
using std::max;
using std::min;
template <class T> [[nodiscard]] auto sq(T x) { return x * x; };
using std::pow;
using std::sqrt;

// Arduino Trigonometry functions
using std::cos;
using std::sin;
using std::tan;

// Arduino Characters functions
SMCE__DLL_RT_API bool isAlpha(char thisChar);
SMCE__DLL_RT_API bool isAlphaNumeric(char thisChar);
SMCE__DLL_RT_API bool isAscii(char thisChar);
SMCE__DLL_RT_API bool isControl(char thisChar);
SMCE__DLL_RT_API bool isDigit(char thisChar);
SMCE__DLL_RT_API bool isGraph(char thisChar);
SMCE__DLL_RT_API bool isHexadecimalDigit(char thisChar);
SMCE__DLL_RT_API bool isLowerCase(char thisChar);
SMCE__DLL_RT_API bool isPrintable(char thisChar);
SMCE__DLL_RT_API bool isPunct(char thisChar);
SMCE__DLL_RT_API bool isSpace(char thisChar);
SMCE__DLL_RT_API bool isUpperCase(char thisChar);
SMCE__DLL_RT_API bool isWhitespace(char thisChar);

// Arduino Random Numbers functions
SMCE__DLL_RT_API long random(long max);
SMCE__DLL_RT_API long random(long min, long max);
SMCE__DLL_RT_API void randomSeed(unsigned long seed);

// Arduino Bits and Bytes functions
#define bit(b) (1UL << (b))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitWrite(value, bit, bitvalue) ((bitvalue) ? bitSet((value), (bit)) : bitClear((value), (bit)))
#define highByte(w) ((uint8_t)((w) >> 8))
#define lowByte(w) ((uint8_t)((w) & 0xff))

// Arduino External Interrupts functions
enum InterruptMode { CHANGE = 1, FALLING, RISING  };
SMCE__DLL_RT_API void attachInterrupt(uint8_t interruptNum, void (*userFunc)(), int mode);
SMCE__DLL_RT_API void detachInterrupt(uint8_t interruptNum);

SMCE__DLL_RT_API void interrupts();
SMCE__DLL_RT_API void noInterrupts();

// User defined functions
extern "C" SMCE__DLL_API void setup();
extern "C" SMCE__DLL_API void loop();

#include "WString.h"
#include "HardwareSerial.h"
#endif // Arduino_h
