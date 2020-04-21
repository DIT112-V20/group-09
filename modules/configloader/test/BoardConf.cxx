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
    "digital_pin_count": 36,
    "analog_pin_count": 13,
    "interruptable_digital_pins": [2, 8, 9],
    "pwm_capable_pins": [1, 2, 3, 4, 5],
    "uart_pairs": [{"rx_pin": 3, "tx_pin": 4}, {"rx_pin": 16, "tx_pin": 17}]
}
)");
    const auto res = smce::load(doc);
    REQUIRE(res);
    const auto& cnf = *res;
    REQUIRE(cnf.name == "esp32-dev");
    REQUIRE(cnf.fqbn == "esp32:esp32:esp32");
    REQUIRE(cnf.digital_pin_count == 36);
    REQUIRE(cnf.analog_pin_count == 13);
    REQUIRE(cnf.interruptable_digital_pins == std::vector<std::uint16_t>{2, 8, 9});
    REQUIRE(cnf.pwm_capable_pins == std::vector<std::uint16_t>{1, 2, 3, 4, 5});
    REQUIRE(cnf.uart_pairs.size() == 2);
    REQUIRE((cnf.uart_pairs[0].rx_pin == 3 && cnf.uart_pairs[0].tx_pin == 4));
    REQUIRE((cnf.uart_pairs[1].rx_pin == 16 && cnf.uart_pairs[1].tx_pin == 17));
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