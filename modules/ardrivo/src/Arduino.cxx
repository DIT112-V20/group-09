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

#include <Arduino.h>
#include "BoardData.hxx"

// Arduino Math functions
long constrain(long amt, long low, long high) { return (amt < low ? low : (amt > high ? high : amt)); }
long map(long x, long in_min, long in_max, long out_min, long out_max) { return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; }

// Arduino Characters functions
bool isAlpha(char thisChar) { return std::isalpha(thisChar); }
bool isAlphaNumeric(char thisChar) { return std::isalnum(thisChar); }
bool isAscii(char thisChar) { return static_cast<unsigned char>(thisChar) < 128 }
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
void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode) {
    if (interruptNum < interruptsHandlers.size())
        interruptsHandlers[interruptNum] = std::pair{userFunc, mode};
}
void detachInterrupt(uint8_t interruptNum) {
    if (interruptNum < interruptsHandlers.size())
        interruptsHandlers.erase(interruptsHandlers.begin() + interruptNum);
}