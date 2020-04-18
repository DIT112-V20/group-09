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

#ifndef SMARTCAR_EMUL_ARDUINO_H
#define SMARTCAR_EMUL_ARDUINO_H
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

enum SignificantBit : bool { RIGHTMOSTBIT, LEFTMOSTBIT };

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
void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

// time
void delay(unsigned long duration);
void delayMicroseconds(unsigned int duration);
unsigned long micros();
unsigned long millis();

extern void setup();
extern void loop();

#endif // SMARTCAR_EMUL_ARDUINO_H
