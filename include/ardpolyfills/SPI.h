/*
 *  SPI.h
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

#ifndef SPI_h
#define SPI_h

#include <cstdint>
#include "SMCE__dll.hxx"

enum SMCE__BitOrdering : uint8_t {
    LSBFIRST,
    MSBFIRST
};

enum SMCE__SpiClockDivider : uint8_t {
    SPI_CLOCK_DIV4,
    SPI_CLOCK_DIV16,
    SPI_CLOCK_DIV64,
    SPI_CLOCK_DIV128,
    SPI_CLOCK_DIV2,
    SPI_CLOCK_DIV8,
    SPI_CLOCK_DIV32
};


enum SMCE__SpiMode : uint8_t {
    SPI_MODE0 = 0,
    SPI_MODE1 = 4,
    SPI_MODE2 = 8,
    SPI_MODE3 = 12
};

struct SPISettings {
    constexpr SPISettings() noexcept = default;
    constexpr SPISettings(std::uint8_t clock, std::uint8_t bit_order, std::uint8_t data_mode) noexcept
        : clock_div{clock}, bit_order{bit_order}, data_mode{data_mode} {}
    constexpr SPISettings(SMCE__SpiClockDivider clock, SMCE__BitOrdering bit_order, SMCE__SpiMode data_mode) noexcept
        : clock_div{clock}, bit_order{bit_order}, data_mode{data_mode} {}

    SMCE__SpiClockDivider clock_div{};
    SMCE__BitOrdering bit_order{};
    SMCE__SpiMode data_mode{};
};

class SMCE__DLL_RT_API SPIClass {
    SPISettings settings{};
    std::int16_t slave_sel = -1;
    std::uint8_t bus_id = 0;
    bool active = false;

  public:
    void begin();
    void begin(int slave_select); // Arduino Due only
    void end();
    void beginTransaction(SPISettings spi_settings = {});
    void endTransaction();
    [[deprecated]] void setBitOrder(SMCE__BitOrdering bit_ordering);
    [[deprecated]] void setDataMode(SMCE__SpiMode spi_mode);
    [[deprecated]] void setDataMode(int slave_select, std::uint8_t spi_mode); // Arduino Due only
    [[deprecated]] void setClockDivider(SMCE__SpiClockDivider divider);
    [[deprecated]] void setClockDivider(int slave_select, std::uint8_t divider); // Arduino Due only
    uint8_t transfer(std::uint8_t data);
    uint16_t transfer16(std::uint16_t data);
    void transfer(void* buf, std::uint16_t count);
    bool pins(std::int8_t sck, std::int8_t miso, std::int8_t mosi, std::int8_t ss); // ESP only
};

#ifndef _MSC_VER
extern SMCE__DLL_RT_API SPIClass SPI;
#else
SMCE__DLL_RT_API SPIClass& SMCE__DATA_TRAMPOLINE_SPI() noexcept;
#   ifdef SMCE__COMPILING_USERCODE
#       define SPI SMCE__DATA_TRAMPOLINE_SPI()
#   else
extern SPIClass SPI;
#   endif
#endif

#endif // SPI_h
