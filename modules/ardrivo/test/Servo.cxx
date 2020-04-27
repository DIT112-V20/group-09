#include <catch2/catch.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include "BoardDataDef.hxx"
#include "Entrypoint.hxx"
#include "Servo.h"

static void init_fake() {
    if(board_data)
        (delete board_data), board_data = nullptr;
    if(board_info)
        (delete board_info), board_data = nullptr;

    auto loc_board_data = std::make_unique<BoardData>();
    loc_board_data->pwm_values = std::vector<std::atomic_uint8_t>(255);

    auto loc_board_info = std::make_unique<BoardInfo>();
    loc_board_info->pins_caps.resize(255);
    ranges::for_each(loc_board_info->pins_caps, [](PinCapability& pin) { pin.pwm_able = true; });
    loc_board_info->pins_caps[2].pwm_able = false;

    init(loc_board_data.release(), loc_board_info.release());
}

bool writtenSuccessful(Servo currentServo, int attchedPin, int expectWrittenValue) {
    return currentServo.read() == expectWrittenValue && board_data->pwm_values[attchedPin].load() == expectWrittenValue;
}

TEST_CASE("Attach pin", "[Attach]") {
    init_fake();
    Servo test;
    test.attach(1);
    REQUIRE(test.attached());
    // Attach pin out of the boundary
    test.detach();
    board_data->silence_errors = true;
    test.attach(500);
    REQUIRE(!test.attached());
    // Attach PWM-unable pin
    test.attach(2);
    REQUIRE(!test.attached());
    board_data->silence_errors = false;

    REQUIRE_THROWS_AS(test.attach(500), std::runtime_error);
    REQUIRE_THROWS_AS(test.attach(2), std::runtime_error);
}

TEST_CASE("Attached", "[Attached]") {
    init_fake();
    Servo test;
    REQUIRE(!test.attached());
    test.attach(1);
    REQUIRE(test.attached());
}

TEST_CASE("Detach pin", "[Detach]") {
    init_fake();
    Servo test;
    test.attach(1);
    REQUIRE(test.attached());
    test.detach();
    REQUIRE(!test.attached());
}

TEST_CASE("Write servo value", "[Write]") {
    init_fake();
    Servo test;
    // Unattached case
    test.write(100);
    REQUIRE(test.read() == -1);
    // Attached cases
    test.attach(1);
    REQUIRE(test.attached());
    test.write(100); // Inside boundary
    REQUIRE(writtenSuccessful(test, 1, 100));
    test.write(-100); // Lower than boundary
    REQUIRE(writtenSuccessful(test, 1, 0));
    test.write(1000); // Higher than boundary
    REQUIRE(writtenSuccessful(test, 1, 180));
}

TEST_CASE("Write servo value as MicroSeconds", "[WriteMicroseconds]") {
    init_fake();
    Servo test;
    // Unattached case
    test.write(1500);
    REQUIRE(test.read() == -1);
    // Attached cases
    test.attach(1);
    REQUIRE(test.attached());
    test.writeMicroseconds(1500); // Inside boundary
    REQUIRE(writtenSuccessful(test, 1, 89));
    test.writeMicroseconds(500); // Lower than boundary
    REQUIRE(writtenSuccessful(test, 1, 0));
    test.writeMicroseconds(2500); // Higher than boundary
    REQUIRE(writtenSuccessful(test, 1, 179));
}

TEST_CASE("Read servo value", "[Read]") {
    init_fake();
    Servo test;
    // Unattached case
    test.write(90);
    REQUIRE(test.read() == -1);
    // Attached case
    test.attach(1);
    REQUIRE(test.attached());
    test.write(90);
    REQUIRE(writtenSuccessful(test, 1, 90));
}
