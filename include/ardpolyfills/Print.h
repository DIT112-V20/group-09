/*
 *  Print.h
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

#ifndef Print_h
#define Print_h

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include "WString.h"

class Print {
    int write_error = 0;

    /**
    * Writes an error
    **/
  protected:
    constexpr void setWriteError(int err = 1) noexcept { write_error = err; }

  public:
    Print() noexcept = default;
    /**
    * Get the error that is written 
    **/
    [[nodiscard]] constexpr int getWriteError() noexcept { return write_error; }

    /**
    * Clears the error written
    **/
    constexpr void clearWriteError() noexcept { setWriteError(0); }

    /**
    * Gets overridden by subclasses 
    **/
    virtual std::size_t write(std::uint8_t) = 0;

    /**
    * Returns the number of bytes that was written 
    **/
    inline virtual std::size_t write(const uint8_t* buffer, std::size_t size) {
        const auto beg = buffer;
        while (size-- && write(*buffer++))
            ;
        return std::distance(beg, buffer);
    }

    /**
    * Returns the value from write and terminates the null characters in str
    **/
    inline std::size_t write(const char* str) {
        if (!str)
            return 0;
        return write(str, std::strlen(str));
    }

    /**
    * Converts between types (char and uint8_t) and returns the value from write
    **/
    inline std::size_t write(const char* buffer, size_t size) { return write(reinterpret_cast<const std::uint8_t*>(buffer), size); }

    // default to zero, meaning "a single write may block"
    // should be overriden by subclasses with buffering
    /**
    * Gets overridden by subclasses
    **/
    inline virtual int availableForWrite() { return 0; }

    // template <std::size_t N>
    // std::size_t print(const char (&lit)[N]) { return write(lit, N); }

    /**
    * 
    **/
    inline std::size_t print(const String& s) { return write(s.c_str(), s.length()); }
    inline std::size_t print(const char* czstr) { return write(czstr); }
    inline std::size_t print(char c) { return write(c); }
    template <class Int, class = std::enable_if_t<std::is_integral<Int>::value>> inline std::size_t print(Int val, StringBaseConv base = DEC) {
        return print(String(val, base));
    }
    // inline std::size_t print(double val, int prec = 2) { return print(String(val, prec)); }
    std::size_t print(const struct Printable&); // FIXME: implement base Printable

    template <std::size_t N> std::size_t println(const char (&lit)[N]) { return write(lit, N) + println(); }
    inline std::size_t println(const String& s) { return print(s) + println(); }
    inline std::size_t println(const char* czstr) { return write(czstr) + println(); }
    inline std::size_t println(char c) { return write(c) + println(); }
    template <class Int, class = std::enable_if_t<std::is_integral<Int>::value>> std::size_t println(Int val, StringBaseConv base = DEC) {
        return print(val, base) + println();
    }
    // inline std::size_t println(double val, int prec = 2) { return print(val, prec) + println(); }
    inline std::size_t println(const Printable& p) { return print(p) + println(); }
    inline std::size_t println() { return print('\r') + print('\n'); }

    inline virtual void flush() {} // Empty implementation for backward compatibility
};

#endif // Print_h
