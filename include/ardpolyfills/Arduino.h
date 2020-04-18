/*
 *  Arduino.h
 *  Copyright 2020 ItJustWorkTM
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
void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode);
void detachInterrupt(uint8_t interruptNum);
#endif // Arduino_h