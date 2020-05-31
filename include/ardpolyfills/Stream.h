/*
 *  Stream.h
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

#ifndef Stream_h
#define Stream_h
#include "SMCE__dll.hxx"
#include "Print.h"

#define NO_IGNORE_CHAR '\x01'
#define DEFAULT_TIMEOUT 1000

/// Used in switch case for the different cases
enum LookaheadMode { SKIP_ALL, SKIP_NONE, SKIP_WHITESPACE };

struct Stream : Print {

struct SMCE__DLL_RT_API Stream : Print {

    ///Defaul wating time to wait for a stream data
    long _timeout{DEFAULT_TIMEOUT};

  protected:
    int peekNextDigit(LookaheadMode lookahead, bool detectDecimal);

  public:
    /**
    * Gets overridden by subcass
    **/
    virtual int available() = 0;

    /**
    * Gets overridden by subcass
    **/
    virtual int read() = 0;

    /**
    * Gets overridden by subcass
    **/
    virtual int peek() = 0;

    Stream() = default;

    /**
    * Calls find(char, size_t)
    **/
    [[nodiscard]] bool find(char target) noexcept { return find(&target, 0); }

    /**
    * Calls finduntil(char, char)
    **/
    [[nodiscard]] bool find(char* target, size_t length) noexcept { return findUntil(target, length, NO_IGNORE_CHAR); }

    /**
    * Calls findUntil(char, char, char)
    **/
    [[nodiscard]] bool findUntil(char target, char terminal) noexcept { return findUntil(&target, 0, NO_IGNORE_CHAR); }

    /**
    * Reads data from the stream until the target string of given length or terminator string is found
    **/
    [[nodiscard]] bool findUntil(char* target, int length, char terminal) noexcept;

    /**
    * Calls readBytesUntil()
    **/
    size_t readBytes(char* buffer, int length) { return readBytesUntil(NO_IGNORE_CHAR, buffer, length); }

    /**
    *  Read characters from a stream into a buffer and terminate upon reached char
    **/
    size_t readBytesUntil(char character, char* buffer, int length);

    /**
    * Calls readStringUntil
    **/
    String readString() { return readStringUntil(NO_IGNORE_CHAR); }

    /**
    *  Reads characters from a stream into a String and terminate upon reached char
    **/
    String readStringUntil(char terminator);

    /**
    * Returns the first valid long number from the current position
    **/
    long parseInt(LookaheadMode lookahead = SKIP_ALL, char ignore = NO_IGNORE_CHAR);

    /**
    * Returns the first valid floating point number from the current position
    **/
    float parseFloat(LookaheadMode lookahead = SKIP_ALL, char ignore = NO_IGNORE_CHAR);

    /**
    * Sets the maximum milliseconds to wait for stream data
    **/
    void setTimeout(long time);
};

#endif // Stream_h
