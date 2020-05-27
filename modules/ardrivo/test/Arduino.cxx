
#include <catch2/catch.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include "Entrypoint.hxx"
#include "BoardDataDef.hxx"
#include "Arduino.h"

using namespace std::literals;

void static init_fake() 
{
    if (board_data)
        (delete board_data), board_data = nullptr;
    if (board_info)
        (delete board_info), board_data = nullptr;

    auto loc_board_data = std::make_unique<BoardData>();
    loc_board_data->silence_errors = false;
    loc_board_data->start_time = std::chrono::steady_clock::now();
    loc_board_data->pin_modes = std::vector<std::atomic_uint8_t>(3);
    loc_board_data->digital_pin_values = std::vector<std::atomic_bool>(3);
    loc_board_data->analog_pin_values = std::vector<std::atomic_uint16_t>(3);
    loc_board_data->pin_frequency = std::vector<std::atomic_uint8_t>(3);
    loc_board_data->interrupts_handlers = std::vector<std::pair<void (*)(), int>>(3);
    auto loc_board_info = std::make_unique<BoardInfo>();
    loc_board_info->pins_caps.resize(3);
    ranges::for_each(loc_board_info->pins_caps, [](PinCapability& pin) {
        pin.digital_in = pin.digital_out = true;
        pin.analog_in = pin.analog_out = true;
        pin.interrupt_able = pin.interrupt_able = true;
    });

    init(loc_board_data.release(), loc_board_info.release());
}

TEST_CASE("pulse in", "[pulseIn success], [pulseIn fail]") {
    init_fake();

    SECTION("pulseIn proper pulse", "[pulseIn success]") {
        auto test_pulse = [&](auto state, auto pin, auto delay) {
            board_data->digital_pin_values[pin] = state;
            auto set_pulse_tr = std::thread([&]() {
                std::this_thread::sleep_for(delay);
                board_data->digital_pin_values[pin] = !state;
                std::this_thread::sleep_for(delay);
                board_data->digital_pin_values[pin] = state;
            });
            REQUIRE(pulseIn(pin, state, delay.count() * 2) > delay.count() * 0.7);
            set_pulse_tr.join();
        };

        test_pulse(HIGH, 0, std::chrono::microseconds(2000000));
        test_pulse(LOW, 0, std::chrono::microseconds(2000000));
    }

    SECTION("pulseIn timeout", "[pulseIn fail]") {
        board_data->digital_pin_values[0] = HIGH;
        REQUIRE(!pulseIn(0, HIGH, 1));
    }
}

TEST_CASE("Read and write digital & analog & pinMode", "[pinMode], [digitalWrite], [digitalRead], [analogWrite], [analogRead]") {
    init_fake();

    SECTION("Set pinModes", "[pinMode]") 
    {         
        auto i = GENERATE(0, 1, 2);
        board_data->pin_modes[i] = static_cast<uint8_t>(PinMode::OUTPUT);
        REQUIRE(board_data->pin_modes[i] == 1U);    
    }

    SECTION("Set digital pins ") 
    {
        digitalWrite(1, LOW);
        board_data->pin_modes[1] = static_cast<uint8_t>(PinMode::INPUT);
        REQUIRE(digitalRead(1) == LOW);
        board_data->pin_modes[1] = static_cast<uint8_t>(PinMode::OUTPUT);
        digitalWrite(1, HIGH);
        board_data->pin_modes[1] = static_cast<uint8_t>(PinMode::INPUT);
        REQUIRE(digitalRead(1) == HIGH);

        auto i = GENERATE(0, 1, 2);
        digitalWrite(i, HIGH);
        REQUIRE(digitalRead(i) == HIGH);     
    }
    SECTION("Set analog pins") {
        board_data->pin_modes[1] = static_cast<uint8_t>(PinMode::OUTPUT);
        analogWrite(1, 0);
        board_data->pin_modes[1] = static_cast<uint8_t>(PinMode::INPUT);
        REQUIRE(analogRead(1) == 0);
        board_data->pin_modes[1] = static_cast<uint8_t>(PinMode::OUTPUT);
        analogWrite(1, 500);
        board_data->pin_modes[1] = static_cast<uint8_t>(PinMode::INPUT);
        REQUIRE(analogRead(1) == 500);
    }

    SECTION("Set value of pins over vector size, digitalWrite and analogWrite") 
    {
        REQUIRE_THROWS_WITH(digitalWrite(32, HIGH), "digitalWrite(32, true): Pin does not exist");
        REQUIRE_THROWS_WITH(analogWrite(32, 100), "analogWrite(32, 100): Pin does not exist");
    }
}

TEST_CASE("Set tone frequency on pin", "[tone], [noTone]")
{
    init_fake();
    auto i = GENERATE(0, 1, 2);
    board_data->pin_frequency[i] = 30;
    REQUIRE(board_data->pin_frequency[i] == 30);
}

TEST_CASE("Shifts in a byte of data one bit at a time, start from either the leftmost or rightmost significant bit" "[shiftIn]")
{
    digitalWrite(1, HIGH);
    digitalWrite(2, HIGH);
    REQUIRE(shiftIn(1, 2, RIGHTMOSTBIT) == 255);
    REQUIRE(!digitalRead(2));

    digitalWrite(1, LOW);
    digitalWrite(2, LOW);
    REQUIRE(shiftIn(1, 2, LEFTMOSTBIT) == 0);
}

TEST_CASE("Shifts out a byte of data one bit at a time, start from either the leftmost or rightmost significant bit" "[shiftOut]" )
{
    digitalWrite(1, HIGH);
    digitalWrite(2, HIGH);
    shiftOut(1, 2, 0, 1);
    REQUIRE(digitalRead(2) == LOW);
    REQUIRE(digitalRead(1) == LOW);
    shiftOut(1, 2, 1, 1);
    REQUIRE(digitalRead(1) == HIGH);
    shiftOut(1, 2, 0, 0);
    REQUIRE(digitalRead(1) == LOW);
}

TEST_CASE("Delay the program", "[delay]") 
{
    const auto start = std::chrono::steady_clock::now();
    delay(1000);
    const auto end = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    REQUIRE(duration > 0s);
    REQUIRE(duration < 1.5s);
}

TEST_CASE("delay the thread in microseconds", "[delayMicroseconds]") 
{
    const auto start = std::chrono::steady_clock::now();
    delayMicroseconds(1000);
    const auto end = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast< std::chrono::microseconds >(end - start);    
    REQUIRE(duration > 1000us);
}

TEST_CASE("Return number of microseconds since arduino board began", "[micros]") {
    init_fake();
    const unsigned long durationBoardBegun = micros();
    const auto start = std::chrono::steady_clock::now();
    delay(1000);
    const auto end = std::chrono::steady_clock::now();
    const auto durationCheck = std::chrono::duration_cast< std::chrono::microseconds >(end - start);
    auto durationBoardBegun2 = micros();    
    const unsigned long totalDurationBegun = (durationBoardBegun2 - durationBoardBegun);
    REQUIRE(totalDurationBegun > 1000100); 
    REQUIRE(durationCheck > 1000100us);
}

TEST_CASE("Return number of milliseconds since arduino board began", "[millis]") {
    init_fake();
    const unsigned long durationBoardBegun = millis();
    const auto start = std::chrono::steady_clock::now();
    delay(1000);
    const auto end = std::chrono::steady_clock::now();
    const auto durationCheck = std::chrono::duration_cast< std::chrono::milliseconds >(end - start);
    auto durationBoardBegun2 = micros();
    const unsigned long totalDurationBegun = (durationBoardBegun2 - durationBoardBegun);
    REQUIRE(durationCheck >= 1000ms);
    REQUIRE(totalDurationBegun >= 1000);
}

TEST_CASE("Constrain number within a given range", "[constrain]") {
    REQUIRE(constrain(5, 2, 10) == 5);
    REQUIRE(constrain(1, 2, 10) == 2);
    REQUIRE(constrain(11, 2, 10) == 10);
}

TEST_CASE("Re-maps number from one range to another" "[map]") {
    REQUIRE(map(1023, 0, 1023, 0, 255) == 255);
}

TEST_CASE("Check is char i letter", "[isAlpha]") {
    REQUIRE(isAlpha('a'));
    REQUIRE(!isAlpha('1'));
}

TEST_CASE("Check if a char is alphanumeric", "[isAlphaNumeric]") {
    REQUIRE(isAlphaNumeric('a'));
    REQUIRE(isAlphaNumeric('1'));
    REQUIRE(!isAlphaNumeric('!'));
}

TEST_CASE("Is the character a ascii", "[isAscii]") {
    char num = 'a';
    REQUIRE(isAscii(num)); 
}

TEST_CASE("Check if the character a control character", "[isControl") {
    REQUIRE(isControl('\t'));
    REQUIRE(isControl('\n'));
    REQUIRE(isControl('\v'));
}
TEST_CASE("Checks if the charahcter a digit" "[isDigit]") {
    REQUIRE(isDigit('1'));
    REQUIRE(!isDigit('a'));
    REQUIRE(!isDigit('#'));
}

TEST_CASE("Checks if a character is a digit" "[isDigit]") {
    REQUIRE(isDigit('1'));
    REQUIRE(!isDigit('a'));
    REQUIRE(!isDigit('#'));
}

TEST_CASE("Checks if a character is a graphical represention" "[isGraph]") {
    REQUIRE(isGraph('a'));
    REQUIRE(isGraph('1'));
    REQUIRE(!isGraph(' '));
}

TEST_CASE("Checks if a character is hexadecimal", "[isHexadecimalDigit]") {
    REQUIRE(isHexadecimalDigit('a'));
    REQUIRE(!isHexadecimalDigit('!'));
}

TEST_CASE("Checks if a character is lowercase", "[isLowerCase") {
    REQUIRE(isLowerCase('a'));
    REQUIRE(!isLowerCase('A'));
}

TEST_CASE("Checks if a character is printable", "[isPrintable]") {
    REQUIRE(isPrintable('a'));
    REQUIRE(isPrintable('1'));
    REQUIRE(isPrintable(' '));
}

TEST_CASE("Checks if a character is a punctuation" "[isPunct") {
    REQUIRE(!isPunct('a'));
    REQUIRE(isPunct(','));
    REQUIRE(isPunct('?'));
}

TEST_CASE("Checks if a character contains white-space", "[isSpace]") {
    REQUIRE(!isSpace('a'));
    REQUIRE(isSpace('\t'));
    REQUIRE(isSpace(' '));
}

TEST_CASE("Checks if a character is uppercase" "[isUpperCase") 
{
    REQUIRE(isUpperCase('A'));
    REQUIRE(!isUpperCase('a'));
}

TEST_CASE("Checks if a character is a blank character" "[isWhiteSpace]") 
{
    REQUIRE(isWhitespace(' '));
    REQUIRE(isWhitespace('\t'));
    REQUIRE(!isWhitespace('a'));
}

TEST_CASE("Checks if it returns a random number within a given range" "[isRandom]") 
{
    auto i = GENERATE(take(100, filter([](int i) { return i; }, random(-100, 100))));
    REQUIRE(i < 100);
    REQUIRE(i > -100);
}


TEST_CASE("Test randomSeed", "[randomSeed]") 
{
    std::array <long, 100> arr {};
    std::array <long, 100> arr2 {};
    std::array <long, 100> arr3 {};

    auto count = 0;
    auto iterator = 0;
    randomSeed(2);

    std::generate(arr.begin(), arr.end(), [](){ return random(100); });
    randomSeed(3);
    std::generate(arr2.begin(), arr2.end(), [](){ return random(100); });

    for (int i = 0; i < 100; i++)
    {
        if (arr == arr2)
        {
            count++;
        }
    }
    REQUIRE(count < 3);

    randomSeed(2);

    std::generate(arr3.begin(), arr3.end(), [](){ return random(100); });

    for (int i = 0; i < 100; i++)
    {
        REQUIRE(arr == arr3);
    }
}
 
void fun() {}

TEST_CASE("Check if vector contains interupt", "[attachInterrupt]") 
{  
    // First test
    auto (*user_func)() = &fun;
    attachInterrupt(0, (user_func), 1);
    auto test = board_data->interrupts_handlers[0];
    REQUIRE(test.first != NULL);
    REQUIRE(test.second == 1);
    
    // Second test
    attachInterrupt(2, (*user_func), 2);
    auto test2 = board_data->interrupts_handlers[2];
    REQUIRE(test2.first != NULL);
    REQUIRE(test2.second == 2);
}

TEST_CASE("Delete a interupt from vector", "[detachInterrupt]") 
{
    auto test = board_data->interrupts_handlers[0];
    REQUIRE(test.first != NULL);
    detachInterrupt(0);
    test = board_data->interrupts_handlers[0];
    REQUIRE(test.first == NULL);
    REQUIRE(test.second == 0);
    auto test2 = board_data->interrupts_handlers[1];
    REQUIRE(test2.first != NULL);
    REQUIRE(test2.second == 2);
}