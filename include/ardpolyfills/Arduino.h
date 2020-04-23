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
#include <cmath>
#include <cstdlib>
#include <algorithm>

enum SignificantBit : bool { RIGHTMOSTBIT, LEFTMOSTBIT };

#ifdef SMCE__COMPILING_ARDUINO_CXX
#   define SMCE__PINMODE_ENUM_STRUCT struct
#else
#   define SMCE__PINMODE_ENUM_STRUCT
#endif

enum SMCE__PINMODE_ENUM_STRUCT PinMode : std::uint8_t { INPUT, OUTPUT, INPUT_PULLUP };

enum DigitalValue : bool { LOW, HIGH };

// Digital and analog i/o
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, bool val);
int digitalRead(uint8_t pin);
int analogRead(uint8_t pin);
void analogReference(uint8_t mode);
void analogWrite(uint16_t pin, uint16_t val);

// advanced i/o
void tone(uint8_t pin, unsigned int frequency, unsigned long duration);
void noTone(uint8_t _pin);
unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout);
void shiftOut(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order, uint8_t val);
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

// time
void delay(unsigned long duration);
void delayMicroseconds(unsigned int duration);
unsigned long micros();
unsigned long millis();

// Arduino Math functions
using std::abs;
long constrain(long amt, long low, long high);
long map(long x, long in_min, long in_max, long out_min, long out_max);
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
bool isAlpha(char thisChar);
bool isAlphaNumeric(char thisChar);
bool isAscii(char thisChar);
bool isControl(char thisChar);
bool isDigit(char thisChar);
bool isGraph(char thisChar);
bool isHexadecimalDigit(char thisChar);
bool isLowerCase(char thisChar);
bool isPrintable(char thisChar);
bool isPunct(char thisChar);
bool isSpace(char thisChar);
bool isUpperCase(char thisChar);
bool isWhitespace(char thisChar);

// Arduino Random Numbers functions
long random(long max);
long random(long min, long max);
void randomSeed(unsigned long seed);

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
void attachInterrupt(uint8_t interruptNum, void (*userFunc)(), int mode);
void detachInterrupt(uint8_t interruptNum);

// User defined functions
extern void setup();
extern void loop();

#include "WString.h"
#include "HardwareSerial.h"
#endif // Arduino_h
