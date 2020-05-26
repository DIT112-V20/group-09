/*
 *  String.cxx
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

#include <range/v3/algorithm/transform.hpp>
#include "WString.h"

[[nodiscard]] const char* String::c_str() const noexcept { return m_u.c_str(); }
[[nodiscard]] std::size_t String::length() const noexcept { return m_u.length(); }
[[nodiscard]] auto String::charAt(unsigned idx) const noexcept { return m_u.at(idx); }
[[nodiscard]] auto& String::charAt(unsigned idx) noexcept { return m_u.at(idx); }
[[nodiscard]] auto String::operator[](unsigned idx) const noexcept { return m_u[idx]; }
[[nodiscard]] auto& String::operator[](unsigned idx) noexcept { return m_u[idx]; }

[[nodiscard]] int String::compareTo(const String& s) const noexcept {
    return std::memcmp(m_u.c_str(), s.m_u.c_str(), (std::min)(s.m_u.size(), m_u.size()));
}

[[nodiscard]] bool String::startsWith(const String& s) const noexcept {
    if (s.m_u.size() > m_u.size())
        return false;
    return std::memcmp(m_u.c_str(), s.m_u.c_str(), s.m_u.size()) == 0;
}

[[nodiscard]] bool String::endsWith(const String& s) const noexcept {
    if (s.m_u.size() > m_u.size())
        return false;
    return (m_u.compare(m_u.length() - s.m_u.length(), s.m_u.length(), s.m_u) == 0);
}

void String::getBytes(byte* buffer, unsigned length) const noexcept{
    std::transform(m_u.begin(), (length > m_u.length()) ? m_u.end() : m_u.begin() + length, buffer, [](unsigned char c) -> byte { return c; });
}

[[nodiscard]] int String::indexOf(const char* c) const noexcept { return m_u.find(c); }

[[nodiscard]] int String::indexOf(const char* c, unsigned index) const noexcept { return m_u.find(c, index); }

[[nodiscard]] int String::indexOf(const String& str) const noexcept { return m_u.find(str.m_u); }

[[nodiscard]] int String::indexOf(const String& str, unsigned index) const noexcept { return m_u.find(str.m_u, index); }

void String::remove(unsigned idx) { m_u.erase(idx); }

void String::remove(unsigned idx, unsigned count) { m_u.erase(idx, idx + count - 1); }

void String::replace(const String& substring1, const String& substring2) {
    size_t position = m_u.find(substring1.m_u);

    while (position != std::string::npos) {
        m_u.replace(position, m_u.size(), substring2.m_u);
        position = m_u.find(substring1.m_u, position + substring2.m_u.size());
    }
}

void String::reserve(unsigned size) { m_u.reserve(size); }

void String::setCharAt(unsigned index, char c) { m_u[index] = c; }

[[nodiscard]] String String::substring(unsigned from) const { return m_u.substr(from); }

[[nodiscard]] String String::substring(unsigned from, unsigned to) const { return m_u.substr(from, to - from); }

void String::toCharArray(char* buffer, unsigned length) noexcept {
    std::memcpy(buffer, m_u.c_str(), (std::min)(static_cast<std::size_t>(length), m_u.length()));
}

[[nodiscard]] long String::toInt() const noexcept try { return std::stoi(m_u); } catch (const std::exception& e) {
    return 0;
}

[[nodiscard]] double String::toDouble() const noexcept try { return std::stod(m_u); } catch (const std::exception& e) {
    return 0;
}

[[nodiscard]] float String::toFloat() const noexcept try { return std::stof(m_u); } catch (const std::exception& e) {
    return 0;
}

void String::toLowerCase() noexcept{ std::transform(m_u.begin(), m_u.end(), m_u.begin(),
        [] (char c) { return static_cast<char>(std::tolower(+c)); });
}

void String::toUpperCase() noexcept{ std::transform(m_u.begin(), m_u.end(), m_u.begin(),
       [](char c) { return static_cast<char>(std::toupper(+c)); });
}

void String::trim() { m_u.erase(std::remove(m_u.begin(), m_u.end(), ' '), m_u.end()); }

[[nodiscard]] bool String::equals(const String& s) const noexcept { return m_u == s.m_u; }

[[nodiscard]] bool String::equalsIgnoreCase(const String& s) const noexcept {
    std::string str_a = m_u;
    std::string str_b = s.m_u;
    ranges::transform(str_a, str_a.begin(), [] (char c) { return static_cast<char>(std::tolower(+c)); });
    ranges::transform(str_b, str_b.begin(), [] (char c) { return static_cast<char>(std::tolower(+c)); });
    return str_a == str_b;
}

[[nodiscard]] bool String::operator==(const String& s) const noexcept { return m_u == s.m_u; }
[[nodiscard]] bool String::operator!=(const String& s) const noexcept { return m_u != s.m_u; }
[[nodiscard]] bool String::operator<(const String& s) const noexcept { return m_u < s.m_u; }
[[nodiscard]] bool String::operator<=(const String& s) const noexcept { return m_u <= s.m_u; }
[[nodiscard]] bool String::operator>(const String& s) const noexcept { return m_u > s.m_u; }
[[nodiscard]] bool String::operator>=(const String& s) const noexcept { return m_u >= s.m_u; }

[[nodiscard]] String operator+(const String& lhs, const String& rhs) { return {lhs.m_u + rhs.m_u}; }
[[nodiscard]] String operator+(const String& lhs, const char* rhs) { return {lhs.m_u + rhs}; }
[[nodiscard]] String operator+(const char* lhs, const String& rhs) { return {lhs + rhs.m_u}; }