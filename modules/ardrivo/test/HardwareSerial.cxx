#include <iostream>
#include <vector>
#include <catch2/catch.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include "BoardDataDef.hxx"
#include "Entrypoint.hxx"
#include "HardwareSerial.h"

void static init_fake() {
    HardwareSerial test;
    if (board_data)
        (delete board_info), board_info = nullptr;
    if (board_info)
        (delete board_info), board_info = nullptr;

    auto loc_board_data = std::make_unique<BoardData>();
    loc_board_data->silence_errors = false;
    loc_board_data->uart_buses = std::vector<UartBus>(3);
    loc_board_data->write_byte = +[](unsigned char c) {
        board_data->uart_buses[0].rx[0] = static_cast<std::byte>(c);
        board_data->uart_buses[2].rx[2] = static_cast<std::byte>(c);

        return true;
    };

    auto loc_board_info = std::make_unique<BoardInfo>();
    ranges::for_each(loc_board_data->uart_buses, [](DynaBufferBus& bus) {
        bus.rx.resize(3);
        bus.tx.resize(3);
    });

    init(loc_board_data.release(), loc_board_info.release());
}

TEST_CASE("Hardwareserial begin and write"
          "[begin], [write]") {
    init_fake();

    HardwareSerial test;
    // Check begin
    int failed = test.write(1);
    REQUIRE(failed == 0);

    // Check if begun == true && write return 1;
    test.begin(1);
    int succes = test.write(150);
    REQUIRE(succes == 1);
    REQUIRE(static_cast<uint8_t>(board_data->uart_buses[0].rx[0]) == 150);
    REQUIRE(static_cast<uint8_t>(board_data->uart_buses[2].rx[2]) == 150);
    REQUIRE(static_cast<uint8_t>(board_data->uart_buses[1].rx[1]) == 0);
}

TEST_CASE("Check available bytes for reading"
          "[available]") {
    init_fake();
    HardwareSerial test;
    REQUIRE(test.available() == 3);
}

TEST_CASE("Check how many avaiable bytes for writing"
          "[availableForWrite]") {
    init_fake();
    HardwareSerial test;
    REQUIRE(test.availableForWrite());
}

TEST_CASE("Check peek of the next byte"
    "[peek]") {
    HardwareSerial test;
    test.begin(1);
    test.write(100);
    int peeked = test.peek();
    REQUIRE(peeked == 100);
    REQUIRE(peeked == test.read());
    REQUIRE(peeked != test.read());
}

TEST_CASE("The first byte"
          "[read]") {
    init_fake();
    HardwareSerial test;
    test.begin(1);
    test.write(50);
    REQUIRE(test.read() == 50);
    REQUIRE(test.read() == 0);
}