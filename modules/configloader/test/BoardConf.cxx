#include <cstdint>
#include <vector>
#include <catch2/catch.hpp>
#include "BoardConf.hxx"

TEST_CASE("Read valid board config", "[BoardConf]") {
    rapidjson::Document doc;
    doc.Parse(R"(
{
    "name": "esp32-dev",
    "fqbn": "esp32:esp32:esp32",
    "pin_count": 39,
    "analog_in_capable_pins": [2, 4, 12, 13, 14, 15, 25, 26, 27, 32, 33, 34, 35, 36, 39],
    "analog_out_capable_pins": [26, 25],
    "digital_in_unable_pins": [6, 7, 8, 9, 10, 11, 20, 24, 28, 29, 30, 31],
    "digital_out_unable_pins": [6, 7, 8, 9, 10, 11, 20, 24, 28, 29, 30, 31, 34, 35, 36, 39],
    "pwm_capable_pins": [1, 2, 3, 4, 5],
    "interruptable_pins": [2, 8, 9],
    "uart_quads": [{"rx_pin": 3, "tx_pin": 4, "cts_pin": 19, "rts_pin": 22}, {"rx_pin": 16, "tx_pin": 17, "cts_pin": 65535, "rts_pin": 65535}],
    "i2c_pairs": [{ "sda_pin": 21, "scl_pin": 22}],
    "spi_quads": [{"mosi_pin": 13, "miso_pin": 12, "clk_pin": 14, "cs_pin": 15}, {"mosi_pin": 23, "miso_pin": 19, "clk_pin": 18, "cs_pin": 5}]
}
)");
    const auto res = smce::load(doc);
    REQUIRE(res);
    const auto& cnf = *res;
    REQUIRE(cnf.name == "esp32-dev");
    REQUIRE(cnf.fqbn == "esp32:esp32:esp32");
    REQUIRE(cnf.pin_count == 39);
    REQUIRE(cnf.digital_in_unable_pins == std::vector<std::uint16_t>{6, 7, 8, 9, 10, 11, 20, 24, 28, 29, 30, 31});
    REQUIRE(cnf.digital_out_unable_pins == std::vector<std::uint16_t>{6, 7, 8, 9, 10, 11, 20, 24, 28, 29, 30, 31, 34, 35, 36, 39});
    REQUIRE(cnf.analog_in_capable_pins == std::vector<std::uint16_t>{2, 4, 12, 13, 14, 15, 25, 26, 27, 32, 33, 34, 35, 36, 39});
    REQUIRE(cnf.analog_out_capable_pins == std::vector<std::uint16_t>{26, 25});
    REQUIRE(cnf.pwm_capable_pins == std::vector<std::uint16_t>{1, 2, 3, 4, 5});
    REQUIRE(cnf.interruptable_pins == std::vector<std::uint16_t>{2, 8, 9});
    REQUIRE(cnf.uart_quads.size() == 2);
    REQUIRE((cnf.uart_quads[0].rx_pin == 3 && cnf.uart_quads[0].tx_pin == 4 && cnf.uart_quads[0].cts_pin == 19 && cnf.uart_quads[0].rts_pin == 22));
    REQUIRE((cnf.uart_quads[1].rx_pin == 16 && cnf.uart_quads[1].tx_pin == 17 && cnf.uart_quads[1].cts_pin == 65535 && cnf.uart_quads[1].rts_pin == 65535));
    REQUIRE(cnf.i2c_pairs.size() == 1);
    REQUIRE((cnf.i2c_pairs[0].sda_pin == 21 && cnf.i2c_pairs[0].scl_pin == 22));
    REQUIRE(cnf.spi_quads.size() == 2);
    REQUIRE((cnf.spi_quads[0].mosi_pin == 13 && cnf.spi_quads[0].miso_pin == 12 && cnf.spi_quads[0].clk_pin == 14 && cnf.spi_quads[0].cs_pin == 15));
    REQUIRE((cnf.spi_quads[1].mosi_pin == 23 && cnf.spi_quads[1].miso_pin == 19 && cnf.spi_quads[1].clk_pin == 18 && cnf.spi_quads[1].cs_pin == 5));
}

TEST_CASE("Reject invalid board config", "[BoardConf]") {
    rapidjson::Document doc;
    doc.Parse(R"(
{
    "name": "avr",
    "fqbn": "",
    "digital_pin_count": "yes",
    "analog_pin_count": 56,
    "interruptable_digital_pins": [1, 2],
    "pwm_capable_pins": []
}
)");
    REQUIRE(!smce::load(doc));
}

TEST_CASE("Reject invalid JSON", "[BoardConf]") {
    rapidjson::Document doc;
    doc.Parse(R"(
{
    [},
}
)");
    REQUIRE(!smce::load(doc));
}