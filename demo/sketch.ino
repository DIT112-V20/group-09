#include <Smartcar.h>

constexpr int GYROSCOPE_OFFSET = 37;
constexpr auto PRINT_INTERVAL  = 100ul;
auto previous_printout         = 0ul;

ServoMotor throttle{27};
ServoMotor steering{13};
AckermanControl control{steering, throttle};

GY50 gyroscope{GYROSCOPE_OFFSET};

HeadingCar car{control, gyroscope};

void setup() {
    Serial.begin(9600);
    car.setSpeed(0);
    car.setAngle(45);
}

void loop() {
    handle_input();
    car.update();
    const auto currentTime = millis();
    if (currentTime >= previous_printout + PRINT_INTERVAL) {
        previous_printout = currentTime;
        Serial.println(car.getHeading());
    }
}

void handle_input() {
    if (Serial.available()) {
        const auto input = Serial.readStringUntil('\n');
        if (input.startsWith("m")) {
            const auto throttle = input.substring(1).toInt();
            car.setSpeed(throttle);
        } else if (input.startsWith("t")) {
            const auto deg = input.substring(1).toInt();
            car.setAngle(deg);
        }
    }
}