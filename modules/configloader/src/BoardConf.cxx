/*
 *  BoardConf.cxx
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

#include <concepts>
#include <fstream>
#include <iterator>
#include <optional>
#include <nameof.hpp>
#include <range/v3/algorithm/transform.hpp>
#include <range/v3/algorithm/for_each.hpp>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wambiguous-reversed-operator"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/schema.h>
#pragma clang diagnostic pop

#include "BoardConf.hxx"
#include "ConfHelper.hxx"

template <std::integral T>
T json_deserialize_integral_unchecked(const rapidjson::Value& jval){
    if constexpr (std::is_same_v<T, bool>)
        return jval.GetBool();
    else if constexpr (std::is_same_v<T, std::uint16_t>)
        return static_cast<T>(jval.GetUint());
    else if constexpr (std::is_same_v<T, std::uint32_t>)
        return jval.GetUint();
    else if constexpr (std::is_same_v<T, std::uint64_t>)
        return jval.GetUint64();
    else if constexpr (std::is_same_v<T, std::int16_t>)
        return static_cast<T>(jval.GetInt());
    else if constexpr (std::is_same_v<T, std::int32_t>)
        return jval.GetInt();
    else if constexpr (std::is_same_v<T, std::int64_t>)
        return jval.GetInt64();
    else
        static_assert(std::is_void_v<std::void_t<T>>, "Unsupported type");
}

template <MpKPair MpKP, class JO>
void transpose_integral(const JO& json_obj, typename MpKP::ClassType& c, MpKP mkp){
    c.*mkp.mptr =  json_deserialize_integral_unchecked<typename MpKP::FieldType>(json_obj[mkp.key]);
};

namespace smce {

[[nodiscard]] std::optional<BoardConf> load(const rapidjson::Document& json_doc) noexcept {

    {
        rapidjson::Document sd;
        if (sd.Parse(board_conf_schema).HasParseError())
            [[unlikely]] return std::nullopt;

        rapidjson::SchemaDocument schema_doc{sd};
        rapidjson::SchemaValidator schema_validator{schema_doc};
        if (!json_doc.Accept(schema_validator))
            [[unlikely]] return std::nullopt;
    }

    std::optional<BoardConf> ret{std::in_place};
    auto& brd = ret.value();

    const auto root = json_doc.GetObject();
    auto pass_str = [&](MptrKeyPair<std::string, BoardConf> mkp) {
        decltype(auto) node = root[mkp.key];
        brd.*mkp.mptr = std::string{node.GetString(), node.GetStringLength()};
    };

    pass_str(ADDR_AND_NAME(BoardConf::name));
    pass_str(ADDR_AND_NAME(BoardConf::fqbn));

    auto pass_integral = [&]<std::integral I>(MptrKeyPair<I, BoardConf> mkp){ transpose_integral(root, brd, mkp); };

    pass_integral(ADDR_AND_NAME(BoardConf::pin_count));

    auto pass_integral_array = [&]<std::integral I>(MptrKeyPair<std::vector<I>, BoardConf> mkp){
        const auto json_arr = root[mkp.key].GetArray();
        (brd.*mkp.mptr).resize(json_arr.Size());
        ranges::transform(json_arr, (brd.*mkp.mptr).begin(), json_deserialize_integral_unchecked<I>);
    };

    pass_integral_array(ADDR_AND_NAME(BoardConf::digital_in_unable_pins));
    pass_integral_array(ADDR_AND_NAME(BoardConf::digital_out_unable_pins));
    pass_integral_array(ADDR_AND_NAME(BoardConf::analog_in_capable_pins));
    pass_integral_array(ADDR_AND_NAME(BoardConf::analog_out_capable_pins));
    pass_integral_array(ADDR_AND_NAME(BoardConf::interruptable_pins));
    pass_integral_array(ADDR_AND_NAME(BoardConf::pwm_capable_pins));

    auto pass_integral_aggregate_array = [&]<MpKPair RMKP, MpKPair... MKPs>(RMKP root_mkp, MKPs... mkps){
      const auto json_arr = root[root_mkp.key].GetArray();
      (brd.*root_mkp.mptr).resize(json_arr.Size());
      ranges::transform(json_arr, (brd.*root_mkp.mptr).begin(), [mkps...](const rapidjson::Value& jval){
            typename RMKP::FieldType::value_type ret;
          (transpose_integral(jval.GetObject(), ret, mkps), ...);
          return ret;
      });
    };

    using Uart = BoardConf::Uart;
    pass_integral_aggregate_array(ADDR_AND_NAME(BoardConf::uart_quads),
                                  ADDR_AND_NAME(Uart::rx_pin),
                                  ADDR_AND_NAME(Uart::tx_pin),
                                  ADDR_AND_NAME(Uart::cts_pin),
                                  ADDR_AND_NAME(Uart::rts_pin));

    using I2c = BoardConf::I2c;
    pass_integral_aggregate_array(ADDR_AND_NAME(BoardConf::i2c_pairs),
                                  ADDR_AND_NAME(I2c::scl_pin),
                                  ADDR_AND_NAME(I2c::sda_pin));

    using Spi = BoardConf::Spi;
    pass_integral_aggregate_array(ADDR_AND_NAME(BoardConf::spi_quads),
                                  ADDR_AND_NAME(Spi::mosi_pin),
                                  ADDR_AND_NAME(Spi::miso_pin),
                                  ADDR_AND_NAME(Spi::clk_pin),
                                  ADDR_AND_NAME(Spi::cs_pin));
    return ret;
}

[[nodiscard]] std::optional<BoardConf> load(const stdfs::path& file_location) noexcept {
    rapidjson::Document doc;
    std::ifstream ifs{file_location};
    if(!ifs)
        [[unlikely]] return std::nullopt;

    rapidjson::IStreamWrapper isw(ifs);
    if(doc.ParseStream(isw).HasParseError())
        [[unlikely]] return std::nullopt;

    return load(doc);
}

[[nodiscard]] BoardData as_data(const BoardConf& conf) noexcept {
    BoardData ret;
    ret.digital_pin_values = std::vector<std::atomic_bool>(conf.pin_count);
    ret.analog_pin_values = std::vector<std::atomic_uint16_t>(conf.pin_count);
    ret.pwm_values = std::vector<std::atomic_uint8_t>(conf.pin_count);
    ret.pin_frequency = std::vector<std::atomic_uint8_t>(conf.pin_count);
    ret.uart_buses = std::vector<UartBus>(conf.uart_quads.size());
    ret.i2c_buses = std::vector<I2cBus>(conf.i2c_pairs.size());
    ret.pin_modes = std::vector<std::atomic_uint8_t>(conf.pin_count);

    return ret;
}

[[nodiscard]] BoardInfo as_info(const BoardConf& conf) noexcept {
    BoardInfo ret;
    ret.pins_caps.resize(conf.pin_count);
    ranges::for_each(ret.pins_caps, [](PinCapability& pin){ pin.digital_in = pin.digital_out = true; }); // default to all digital-able

    for (auto pin : conf.digital_in_unable_pins)
        ret.pins_caps[pin].digital_in = false;
    for (auto pin : conf.digital_out_unable_pins)
        ret.pins_caps[pin].digital_out = false;
    for (auto pin : conf.analog_in_capable_pins)
        ret.pins_caps[pin].analog_in = true;
    for (auto pin : conf.analog_out_capable_pins)
        ret.pins_caps[pin].analog_out = true;
    for (auto pin : conf.pwm_capable_pins)
        ret.pins_caps[pin].pwm_able = true;
    for (auto pin : conf.interruptable_pins)
        ret.pins_caps[pin].interrupt_able = true;

    ret.i2c_chans.resize(conf.i2c_pairs.size());
    ranges::transform(conf.i2c_pairs, ret.i2c_chans.begin(), [](const auto& in) { I2cChannelInfo ret; std::memcpy(&ret, &in, sizeof(in)); return ret; });
    ret.uart_chans.resize(conf.uart_quads.size());
    ranges::transform(conf.uart_quads, ret.uart_chans.begin(), [](const auto& in) { UartChannelInfo ret; std::memcpy(&ret, &in, sizeof(in)); return ret; });
    ret.spi_chans.resize(conf.spi_quads.size());
    ranges::transform(conf.spi_quads, ret.spi_chans.begin(), [](const auto& in) { SpiChannelInfo ret; std::memcpy(&ret, &in, sizeof(in)); return ret; });

    return ret;
}

}