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
SMCE__DLL_RT_API void pinMode(uint8_t pin, uint8_t mode);
/**
* Set a digital pin to high or low
**/
SMCE__DLL_RT_API void digitalWrite(uint8_t pin, bool val);
/**
* Reads a value from a digital pin
**/
SMCE__DLL_RT_API int digitalRead(uint8_t pin);
/**
* Reads a value from an analog pin
**/
SMCE__DLL_RT_API int analogRead(uint8_t pin);
SMCE__DLL_RT_API void analogReference(uint8_t mode);
/**
* Set an analog value to a pin
**/
SMCE__DLL_RT_API void analogWrite(uint16_t pin, uint16_t val);

// advanced i/o
/**
* Set pin to given frequency
**/
SMCE__DLL_RT_API void tone(uint8_t pin, unsigned int frequency, unsigned long duration);
/**
* Set the pin to a no none frequency
**/
SMCE__DLL_RT_API void noTone(uint8_t _pin);
/**
* Reads how long it will take for a pin to go from low to high. Or if the pin is already in high when called
* it waits for it to go down to low
**/
SMCE__DLL_RT_API unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
/**
* Calls the pulseIn function
**/
SMCE__DLL_RT_API unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout);
/**
* Shifts out a byte of data one bit at a time
**/
SMCE__DLL_RT_API void shiftOut(uint8_t data_pin, uint8_t clock_pin, uint8_t bit_order, uint8_t val);
/**
* Shifts in a byte of data one bit at a time
**/
SMCE__DLL_RT_API uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

// time
/**
* Puts the thread to sleep for a given millisecond duration
**/
SMCE__DLL_RT_API void delay(unsigned long duration);
/**
* Puts the thread to sleep for a given microseconds duration
**/
SMCE__DLL_RT_API void delayMicroseconds(unsigned int duration);
/**
* Returns the number of microseconds since the current program began running
**/
SMCE__DLL_RT_API unsigned long micros();
/**
* Returns the number of milliseconds since the current program began running
**/
SMCE__DLL_RT_API unsigned long millis();

// Arduino Math functions
using std::abs;
SMCE__DLL_RT_API long constrain(long amt, long low, long high);
SMCE__DLL_RT_API long map(long x, long in_min, long in_max, long out_min, long out_max);

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
SMCE__DLL_RT_API bool isAlpha(char thisChar);
/**
* Returns true if a char is alphanumeric (that is a letter or a numbers)
**/
SMCE__DLL_RT_API bool isAlphaNumeric(char thisChar);
/**
* Returns true if a char is Ascii
**/
SMCE__DLL_RT_API bool isAscii(char thisChar);
/**
* Returns true if a char is a control character
**/
SMCE__DLL_RT_API bool isControl(char thisChar);
/**
* Returns true if a char is a digit (that is a number)
**/
SMCE__DLL_RT_API bool isDigit(char thisChar);
/**
* Returns true if a char is printable with some content (space is printable but has no content)
**/
SMCE__DLL_RT_API bool isGraph(char thisChar);
/**
* Returns true if a char is an hexadecimal digit (A-F, 0-9)
**/
SMCE__DLL_RT_API bool isHexadecimalDigit(char thisChar);
/**
* Returns true if a char is lower case (that is a letter in lower case)
**/
SMCE__DLL_RT_API bool isLowerCase(char thisChar);
/**
* Returns true if a char is printable (that is any character that produces an output)
**/
SMCE__DLL_RT_API bool isPrintable(char thisChar);
/**
* Returns true if a char is punctuation (that is a comma, a semicolon, an exclamation for example)
**/
SMCE__DLL_RT_API bool isPunct(char thisChar);
/**
* Returns true if a char is a white-space character
**/
SMCE__DLL_RT_API bool isSpace(char thisChar);
/**
* Returns true if a char is upper case (that is, a letter in upper case)
**/
SMCE__DLL_RT_API bool isUpperCase(char thisChar);
/**
* Returns true if a char is a space character
**/
SMCE__DLL_RT_API bool isWhitespace(char thisChar);

// Arduino Random Numbers functions
/**
* The random function generates pseudo-random numbers
**/
SMCE__DLL_RT_API long random(long max);
SMCE__DLL_RT_API long random(long min, long max);
/**
* initializes the pseudo-random number generator
**/
SMCE__DLL_RT_API void randomSeed(unsigned long seed);

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

enum InterruptMode { CHANGE = 1, FALLING, RISING  };

/**
* Stores the interupts in board_data as a single unit
**/
SMCE__DLL_RT_API void attachInterrupt(uint8_t interruptNum, void (*userFunc)(), int mode);
/**
* Detaches the stored interupt
**/
SMCE__DLL_RT_API void detachInterrupt(uint8_t interruptNum);
/**
* Re-enables the interupts
**/
SMCE__DLL_RT_API void interrupts();
/**
* disables the interupts
**/
SMCE__DLL_RT_API void noInterrupts();

// User defined functions
/**
* setup function and loop for ardunio 
**/
extern "C" SMCE__DLL_API void setup();
extern "C" SMCE__DLL_API void loop();

#endif // Arduino_h
