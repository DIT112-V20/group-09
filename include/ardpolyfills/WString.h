/*
 *  WString.h
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
 */

#ifndef WString_h
#define WString_h

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstring>
#include <iostream>
#include <string>

enum StringBaseConv {
    BIN = 2,
    DEC = 10,
    HEX = 16,
};

using byte = std::uint8_t;

class String {
    std::string m_u;

    String(std::string u) : m_u{std::move(u)} {}

  public:
    String() noexcept = default;
    String(const String&) = default;
    String(String&&) noexcept = default;
    String& operator=(const String&) = default;
    String& operator=(String&&) = default;

    template <std::size_t N> inline /* explicit(false) */ String(const char (&charr)[N]) : m_u{charr, N} {}
    inline /* explicit(false) */ String(const char* cstr) : m_u{cstr} {}
    inline explicit String(char c) : m_u(1, c) {}
    template <class T, class = std::enable_if_t<std::is_integral<T>::value>> inline String(T val, StringBaseConv base) {
        constexpr static auto base2len_upperbound = [](StringBaseConv base) /*constexpr*/ {
            switch (base) {
            case BIN:
                return 65;
            case DEC:
                return 11;
            case HEX:
                return 17;
            default:
                throw;
            }
        };
        m_u.resize(base2len_upperbound(base));
        const auto res = std::to_chars(&*m_u.begin(), &*m_u.rbegin(), val, +base);
        if (static_cast<int>(res.ec))
            throw;
        m_u.resize(std::strlen(m_u.c_str()));
    }

    // template <class T, class = std::enable_if_t<std::is_floating_point<T>::value>>
    // String(T val, int precision); // unimplemented

    [[nodiscard]] const char* c_str() const noexcept;
    [[nodiscard]] std::size_t length() const noexcept;
    [[nodiscard]] auto charAt(unsigned idx) const noexcept;
    [[nodiscard]] auto& charAt(unsigned idx) noexcept;
    [[nodiscard]] auto operator[](unsigned idx) const noexcept;
    [[nodiscard]] auto& operator[](unsigned idx) noexcept;

    template <class T> inline bool concat(const T& v) {
        m_u += String(v).m_u;
        return true;
    }

    [[nodiscard]] int compareTo(const String& s) const noexcept;

    [[nodiscard]] bool startsWith(const String& s) const noexcept;

    [[nodiscard]] bool endsWith(const String& s) const noexcept;

    void getBytes(byte* buffer, unsigned length) const noexcept;

    [[nodiscard]] int indexOf(const char* c) const noexcept;

    [[nodiscard]] int indexOf(const char* c, unsigned index) const noexcept;

    [[nodiscard]] int indexOf(const String& str) const noexcept;

    [[nodiscard]] int indexOf(const String& str, unsigned index) const noexcept;

    void remove(unsigned idx);

    void remove(unsigned idx, unsigned count);

    void replace(const String& substring1, const String& substring2);

    void reserve(unsigned size);

    void setCharAt(unsigned index, char c);

    [[nodiscard]] String substring(unsigned from) const;

    [[nodiscard]] String substring(unsigned from, unsigned to) const;

    void toCharArray(char* buffer, unsigned length) noexcept;

    [[nodiscard]] long toInt() const noexcept;

    [[nodiscard]] double toDouble() const noexcept;

    [[nodiscard]] float toFloat() const noexcept;

    void toLowerCase() noexcept;

    void toUpperCase() noexcept;

    void trim();

    friend String operator+(const String&, const String&);
    [[nodiscard]] bool equals(const String& s) const noexcept;
    [[nodiscard]] bool equalsIgnoreCase(const String& s) const noexcept;

    [[nodiscard]] bool operator==(const String& s) const noexcept;
    [[nodiscard]] bool operator!=(const String& s) const noexcept;
    [[nodiscard]] bool operator<(const String& s) const noexcept;
    [[nodiscard]] bool operator<=(const String& s) const noexcept;
    [[nodiscard]] bool operator>(const String& s) const noexcept;
    [[nodiscard]] bool operator>=(const String& s) const noexcept;

    friend String operator+(const String&, const char*);
    friend String operator+(const char*, const String&);
};

[[nodiscard]] String operator+(const String& lhs, const String& rhs);
[[nodiscard]] String operator+(const String& lhs, const char* rhs);
[[nodiscard]] String operator+(const char* lhs, const String& rhs);

#endif