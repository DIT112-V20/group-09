/*
 *  HardwareSerial.cxx
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

#include <iostream>
#include <limits>
#include <sstream>
#include "HardwareSerial.h"

using namespace std;

extern bool (*write_byte)(unsigned char);
extern size_t (*write_buf)(const unsigned char*, size_t);

void HardwareSerial::begin(unsigned long, uint8_t) { begun = true; }

void HardwareSerial::end() { begun = false; }

int HardwareSerial::available() { return std::numeric_limits<int>::max(); }

int HardwareSerial::availableForWrite() { return std::numeric_limits<int>::max(); }

bool HardwareSerial::find(char target) { return std::find(board_data.data.begin(), board_data.data.end(), target) != board_data.data.end(); }

bool HardwareSerial::find(char* target, size_t length) {
    return std::find(board_data.data.begin(), board_data.data.end(), target) != board_data.data.end();
}

bool HardwareSerial::findUntil(char* target, char terminal) {
    int position = 0;
    while (board_data.data.at(position) != terminal) {
        position++;
    }
    return std::find(board_data.data.begin(), board_data.data.begin() + position, target) != board_data.data.end();
}

float HardwareSerial::parseFloat() {
    stringstream ss;
    ss << board_data.data.data();
    string temp;
    float found = 0;
    while (!ss.eof()) {
        ss >> temp;
        if (stringstream(temp) >> found)
            return found;
    }
    return found;
}

float HardwareSerial::parseFloat(std::string lookahead) { return 0; }
float HardwareSerial::parseFloat(std::string lookahead, char ignore) { return 0; }

int HardwareSerial::parseInt() {
    int start, end, position = 0;
    bool found = false;
    while (found == false & position != sizeof(board_data.data)) {
        if (isdigit(board_data.data.at(position))) {
            start = position;
            found = true;
        } else {
            position++;
        }
    }
    if (found == false) {
        return 0;
    }

    if (isdigit(board_data.data.at(position))) {
        position++;
    } else {
        end = position - 1;
    }

    std::string read = board_data.data.data();

    return std::stoi(read.substr(start, end));
}

int HardwareSerial::parseInt(std::string lookahead) { return 0; }
int HardwareSerial::parseInt(std::string lookahead, char ignore) { return 0; }

void HardwareSerial::flush() { std::cout.flush(); }

int HardwareSerial::peek() {
    if (!begun)
        return 0;
    return board_data.data.at(0);
}

size_t HardwareSerial::write(uint8_t c) {
    if (!begun)
        return 0;
    write_byte(c);
    // std::cout.put(c);
    return 1;
}

size_t HardwareSerial::write(const uint8_t* buf, std::size_t n) {
    if (!begun)
        return 0;
    write_buf(buf, n);
    // std::cout.write(reinterpret_cast<const char*>(buf), n);
    return n;
}

int HardwareSerial::read() {
    if (!begun)
        return 0;
    int firstByte = board_data.data.at(0);
    board_data.data.erase(board_data.data.begin());
    return firstByte;
}

int HardwareSerial::readBytes(char buffer[], int length) {
    if (board_data.data.data() == "")
        return 0;
    std::copy(board_data.data.begin(), board_data.data.end(), buffer);
    board_data.data.erase(board_data.data.begin(), board_data.data.end());
    return sizeof(buffer);
}

int HardwareSerial::readBytesUntil(char character, char* buffer, int length) {
    if (board_data.data.data() == "")
        return 0;
    int position = 0;
    while (board_data.data.at(position) != character) {
        buffer[position] = board_data.data.at(position);
        position++;
    }
    board_data.data.erase(board_data.data.begin(), board_data.data.begin() + position);
    return sizeof(buffer);
}

std::string HardwareSerial::readString() {
    std::string read = board_data.data.data();
    board_data.data.erase(board_data.data.begin(), board_data.data.end());
    return read;
}

std::string HardwareSerial::readStringUntil(char terminator) {
    int position = 0;
    std::string read;
    while (board_data.data.at(position) != terminator) {
        read[position] = board_data.data.at(position);
        position++;
    }
    board_data.data.erase(board_data.data.begin(), board_data.data.begin() + position);
    return read;
}

void setTimeout(long duration) { Serial.timeout = duration; }
