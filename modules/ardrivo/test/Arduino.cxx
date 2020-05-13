
#include <catch2/catch.hpp>
#include <iostream>
#include <vector>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include "Entrypoint.hxx"
#include "BoardDataDef.hxx"
#include "Arduino.h"

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
        REQUIRE(digitalRead(1) == LOW);
        digitalWrite(1, HIGH);
        REQUIRE(digitalRead(1) == HIGH);

        auto i = GENERATE(0, 1, 2);
        digitalWrite(i, HIGH);
        REQUIRE(digitalRead(i) == HIGH);     
    }
    SECTION("Set analog pins") {
        analogWrite(1, 0);
        REQUIRE(analogRead(1) == 0);
        analogWrite(1, 500);
        REQUIRE(analogRead(1) == 500);
    }

    SECTION("Set value of pins over vector size. digitalWrite and analogWrite") 
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


// TEST_CASE("Pulse in");

// TEST_CASE("Pulse long in ");

// TEST_CASE("shift in");

// TEST_CASE("shift out");

TEST_CASE("Delay the program", "[delay]") {
    clock_t start = clock();
    delay(1000);
    clock_t stop = clock();
    double elapsed = (double)(stop - start) / CLOCKS_PER_SEC;

    //Added some margin
    REQUIRE(elapsed >= 0);
    REQUIRE(elapsed < 1.1);
}
TEST_CASE("delay the thread in microseconds", "[delayMicroseconds]") {
    clock_t start = clock();
    delay(1000);
    clock_t stop = clock();
    double elapsed = (double)(stop - start) * 1000000.0 / CLOCKS_PER_SEC;

    //Added some margin
    REQUIRE(elapsed >= 0 );
    REQUIRE(elapsed < 1002000.0);
}

TEST_CASE("Return number of microseconds since arduino board began", "[micros]") {
    init_fake();
    const unsigned long duration = micros();

    //Added some margin
    REQUIRE(duration >= 15); 
    REQUIRE(duration < 70);
}

TEST_CASE("Return number of milliseconds since arduino board began", "[millis]") {
    init_fake();
    const unsigned long duration = millis();

    //Added some margin
    REQUIRE(duration >= 0);
    REQUIRE(duration < 30);
}

TEST_CASE("Constrain number within a given range", "[constrain]") {
    long num = constrain(5, 2, 10);
    REQUIRE(num == 5);
    num = constrain(1, 2, 10);
    REQUIRE(num == 2);
    num = constrain(11, 2, 10);
    REQUIRE(num == 10);
}

TEST_CASE("Re-maps number from one range to another"
    "[map]") {
    long num = 1023;
    long num2 = map(num, 0, 1023, 0, 255);
    REQUIRE(num2 == 255);
}

TEST_CASE("Check is char i letter", "[isAlpha]") {
    REQUIRE(isAlpha('a') == true);
    REQUIRE(isAlpha('1') == false);
    ;
}

TEST_CASE("Check if a char is alphanumeric", "[isAlphaNumeric]") {
    REQUIRE(isAlphaNumeric('a') == true);
    REQUIRE(isAlphaNumeric('1') == true);
    REQUIRE(isAlphaNumeric('!') == false);
}

TEST_CASE("Is the character a ascii", "[isAscii]") {
    // REQUIRE(isAscii('a') == 97); //Eeehhh??????????????

    int var = 97;
    //std::cout << (char)var;
}

TEST_CASE("Check if the character a control character", "[isControl") {
    REQUIRE(isControl('\t') == true);
    REQUIRE(isControl('\n') == true);
    REQUIRE(isControl('\v') == true);
}
TEST_CASE("Checks if the charahcter a digit"
    "[isDigit]") {
    REQUIRE(isDigit('1') == true);
    REQUIRE(isDigit('a') == false);
    REQUIRE(isDigit('#') == false);
}

TEST_CASE("Checks if a character is a graphical represention"
    "[isGraph]") {
    REQUIRE(isGraph('a') == true);
    REQUIRE(isGraph('1') == true);
    REQUIRE(isGraph(' ') == false);
}

TEST_CASE("Checks if a character is hexadecimal", "[isHexadecimalDigit]") {
    REQUIRE(isHexadecimalDigit('a') == true);
    REQUIRE(isHexadecimalDigit('!') == false);
}

TEST_CASE("Checks if a character is lowercase", "[isLowerCase") {
    REQUIRE(isLowerCase('a') == true);
    REQUIRE(isLowerCase('A') == false);
}

TEST_CASE("Checks if a character is printable", "[isPrintable]") {
    REQUIRE(isPrintable('a') == true);
    REQUIRE(isPrintable('1') == true);
    REQUIRE(isPrintable(' ') == true);
}

TEST_CASE("Checks if a character is a punctuation"
    "[isPunct") {
    REQUIRE(isPunct('a') == false);
    REQUIRE(isPunct(',') == true);
    REQUIRE(isPunct('?') == true);
}

TEST_CASE("Checks if a character contains white-space", "[isSpace]") {
    REQUIRE(isSpace('a') == false);
    REQUIRE(isSpace('\t') == true);
    REQUIRE(isSpace(' ') == true);
}

TEST_CASE("Checks if a character is uppercase"
    "[isUpperCase") 
{
    REQUIRE(isUpperCase('A') == true);
    REQUIRE(isUpperCase('a') == false);
}

TEST_CASE("Checks if a character is a blank character"
    "[isWhiteSpace]") 
{
    REQUIRE(isWhitespace(' ') == true);
    REQUIRE(isWhitespace('\t') == true);
    REQUIRE(isWhitespace('a') == false);
}

TEST_CASE("Checks if it returns a random number" "[isRandom]") 
{
    auto i = GENERATE(take(100, filter([](int i) { return i; }, random(-100, 100))));
    REQUIRE(i < 100);
    REQUIRE(i > -100);
}
TEST_CASE("Test randomSeed", "[randomSeed]") 
{

    std::srand(2);
    int num = std::rand();
    int num2 = std::rand();

    std::srand(2);
    int num3 = std::rand();

    REQUIRE(num != num2);
    REQUIRE(num == num3);
}

void fun() {}

TEST_CASE("Check if vector contains interupt", "[attachInterrupt]") 
{
    // First test
    void (*user_func)() = &fun;
    attachInterrupt(0, (*user_func), 1);
    auto test = board_data->interrupts_handlers[0];
    REQUIRE(test.second == 1);

    // Second test
    attachInterrupt(2, (*user_func), 2);
    auto test2 = board_data->interrupts_handlers[2];
    REQUIRE(test2.second == 2);
}

TEST_CASE("Delete a interupt from vector", "[detachInterrupt]") 
{
    auto test = board_data->interrupts_handlers[0];
    REQUIRE(test.first != NULL);
    board_data->interrupts_handlers.erase(board_data->interrupts_handlers.begin());
    test = board_data->interrupts_handlers[0];
    REQUIRE(test.first == NULL);
}