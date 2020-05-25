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

enum SignificantBit : bool { RIGHTMOSTBIT, LEFTMOSTBIT };

#ifdef SMCE__COMPILING_ARDUINO_CXX
#   define SMCE__PINMODE_ENUM_STRUCT struct
#else
#   define SMCE__PINMODE_ENUM_STRUCT
#endif
/**
* Enum variables for the configuration of the pins
**/
enum SMCE__PINMODE_ENUM_STRUCT PinMode : std::uint8_t { INPUT, OUTPUT, INPUT_PULLUP };

/**
* Enum variables for the value of the digital pins
**/
enum DigitalValue : bool { LOW, HIGH };

// Digital and analog i/o
/**
* Set the value for a given pin with a configuration
**/
void pinMode(uint8_t pin, uint8_t mode);

/**
* Set a digital pin to high or low
**/
void digitalWrite(uint8_t pin, bool val);

/**
* Reads a value from a digital pin
**/
int digitalRead(uint8_t pin);

/**
* Reads a value from an analog pin
**/
int analogRead(uint8_t pin);
void analogReference(uint8_t mode);

/**
* Set an analog value to a pin
**/
void analogWrite(uint16_t pin, uint16_t val);

// advanced i/o
/**
* Set pin to given frequency
**/
void tone(uint8_t pin, unsigned int frequency, unsigned long duration);

/**
* Set the pin to a no none frequency
**/
void noTone(uint8_t _pin);

/**
* Reads how long it will take for a pin to go from low to high. Or if the pin is already in high when called
* it waits for it to go down to low
**/
unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
/**
* Calls the pulseIn function
**/
unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout);
/**
* Shifts out a byte of data one bit at a time
**/
void shiftOut(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order, uint8_t val);

/**
* Shifts in a byte of data one bit at a time
**/
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

// time
/**
* Puts the thread to sleep for a given millisecond duration
**/
void delay(unsigned long duration);

/**
* Puts the thread to sleep for a given microseconds duration
**/
void delayMicroseconds(unsigned int duration);

/**
* Returns the number of microseconds since the current program began running
**/
unsigned long micros();

/**
* Returns the number of milliseconds since the current program began running
**/
unsigned long millis();

// Arduino Math functions
using std::abs;
/**
* Constrains a number to be within a range
**/
long constrain(long amt, long low, long high);

/**
* Re-maps a number from one range to another
**/
long map(long x, long in_min, long in_max, long out_min, long out_max);

/**
* Calculates the maximum of two numbers
**/
using std::max;

/**
* Calculates the minimum of two numbers
**/
using std::min;

/**
* Calculates the value of a number raised to a power
**/
template <class T> [[nodiscard]] auto sq(T x) { return x * x; };
using std::pow;

/**
* Calculates the square root of a number
**/
using std::sqrt;

// Arduino Trigonometry functions
/**
* Calculates the cosine of an angle (in radians)
**/
using std::cos;

/**
* Calculates the sine of an angle (in radians)
**/
using std::sin;

/**
* Calculates the tangent of an angle (in radians)
**/
using std::tan;

// Arduino Characters functions
/**
* Returns true if the char is a alhpa(that is a letter)
**/
bool isAlpha(char thisChar);

/**
* Returns true if a char is alphanumeric (that is a letter or a numbers)
**/
bool isAlphaNumeric(char thisChar);

/**
* Returns true if a char is Ascii
**/
bool isAscii(char thisChar);

/**
* Returns true if a char is a control character
**/
bool isControl(char thisChar);

/**
* Returns true if a char is a digit (that is a number)
**/
bool isDigit(char thisChar);

/**
* Returns true if a char is printable with some content (space is printable but has no content)
**/
bool isGraph(char thisChar);

/**
* Returns true if a char is an hexadecimal digit (A-F, 0-9)
**/
bool isHexadecimalDigit(char thisChar);

/**
* Returns true if a char is lower case (that is a letter in lower case)
**/
bool isLowerCase(char thisChar);

/**
* Returns true if a char is printable (that is any character that produces an output)
**/
bool isPrintable(char thisChar);

/**
* Returns true if a char is punctuation (that is a comma, a semicolon, an exclamation for example)
**/
bool isPunct(char thisChar);

/**
* Returns true if a char is a white-space character
**/
bool isSpace(char thisChar);

/**
* Returns true if a char is upper case (that is, a letter in upper case)
**/
bool isUpperCase(char thisChar);

/**
* Returns true if a char is a space character
**/
bool isWhitespace(char thisChar);

// Arduino Random Numbers functions
/**
* The random function generates pseudo-random numbers
**/
long random(long max);
long random(long min, long max);

/**
* initializes the pseudo-random number generator
**/
void randomSeed(unsigned long seed);

// Arduino Bits and Bytes functions

/**
* Computes the value of the specified bit 
**/
#define bit(b) (1UL << (b))

/**
* Clears (writes a 0 to) a bit of a numeric variable
**/
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))

/**
* Reads a bit of a number
**/
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)

/**
* Sets (writes a 1 to) a bit of a numeric variable
**/
#define bitSet(value, bit) ((value) |= (1UL << (bit)))

/**
* Writes a bit of a numeric variable
**/
#define bitWrite(value, bit, bitvalue) ((bitvalue) ? bitSet((value), (bit)) : bitClear((value), (bit)))

/**
* Extracts the high-order (leftmost) byte of a word
**/
#define highByte(w) ((uint8_t)((w) >> 8))

/**
* Extracts the low-order (rightmost) byte of a variable
**/
#define lowByte(w) ((uint8_t)((w) & 0xff))

// Arduino External Interrupts functions
/**
* Stores the interupts in board_data as a single unit
**/
enum InterruptMode { CHANGE = 1, FALLING, RISING  };
void attachInterrupt(uint8_t interruptNum, void (*userFunc)(), int mode);

/**
* Detaches the stored interupt
**/
void detachInterrupt(uint8_t interruptNum);

/**
* Re-enables the interupts
**/
void interrupts();

/**
* disables the interupts
**/
void noInterrupts();

// User defined functions
/**
* setup function and loop for ardunio 
**/
extern void setup();
extern void loop();

#include "WString.h"
#include "HardwareSerial.h"
#endif // Arduino_h
