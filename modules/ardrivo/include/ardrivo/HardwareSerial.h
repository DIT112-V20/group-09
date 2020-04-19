/*
 *  HardwareSerial.h
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

#ifndef SMARTCAR_EMUL_HARDWARESERIAL_H
#define SMARTCAR_EMUL_HARDWARESERIAL_H

#include "Print.h"
#include "BoardData.h"
#include <cstddef>
#include <cstdint>

#define SERIAL_5N1 0x00
#define SERIAL_6N1 0x02
#define SERIAL_7N1 0x04
#define SERIAL_8N1 0x06
#define SERIAL_5N2 0x08
#define SERIAL_6N2 0x0A
#define SERIAL_7N2 0x0C
#define SERIAL_8N2 0x0E
#define SERIAL_5E1 0x20
#define SERIAL_6E1 0x22
#define SERIAL_7E1 0x24
#define SERIAL_8E1 0x26
#define SERIAL_5E2 0x28
#define SERIAL_6E2 0x2A
#define SERIAL_7E2 0x2C
#define SERIAL_8E2 0x2E
#define SERIAL_5O1 0x30
#define SERIAL_6O1 0x32
#define SERIAL_7O1 0x34
#define SERIAL_8O1 0x36
#define SERIAL_5O2 0x38
#define SERIAL_6O2 0x3A
#define SERIAL_7O2 0x3C
#define SERIAL_8O2 0x3E

struct HardwareSerial : Print /* FIXME: implement Stream */ {

    BoardData board_data;

    void begin(unsigned long baud) { begin(baud, SERIAL_8N1); }
    void begin(unsigned long, std::uint8_t);
    void end();
    int available();
    //virtual int peek() override;
    //int read() override;
    int availableForWrite() override;
    bool find(char target);
    bool find(char* target, size_t length);
    bool findUntil(char* target, char terminal);
    float parseFloat();
    float parseFloat(std::string lookahead);
    float parseFloat(std::string lookahead, char ignore);
    int parseInt();
    int parseInt(std::string lookahead);
    int parseInt(std::string lookahead, char ignore);
    void flush() override;
    int peek();
    size_t write(std::uint8_t) override;
    size_t write(const std::uint8_t*, size_t) override;
    inline size_t write(unsigned long n) { return write((std::uint8_t)n); }
    inline size_t write(long n) { return write((std::uint8_t)n); }
    inline size_t write(unsigned int n) { return write((std::uint8_t)n); }
    inline size_t write(int n) { return write((std::uint8_t)n); }
    using Print::write; // pull in string printers
    int read();
    int readBytes(char buffer[], int length);
    int readBytesUntil(char character, char buffer[], int length);
    std::string readString();
    std::string readStringUntil(char terminator);

    [[nodiscard]] constexpr explicit(false) operator bool() noexcept { return true; }
    long timeout;

  private:
    bool begun = false;
};

inline HardwareSerial Serial;
#define HAVE_HWSERIAL0

#endif // SMARTCAR_EMUL_HARDWARESERIAL_H
