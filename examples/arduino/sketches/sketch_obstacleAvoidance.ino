#include <Smartcar.h>
#include <VL53L0X.h>
#include <Wire.h>

constexpr float cruise_speed = 40;
constexpr unsigned trigger_dist = 300;

BrushedMotor leftMotor{smartcarlib::pins::v2::leftMotorPins};
BrushedMotor rightMotor{smartcarlib::pins::v2::rightMotorPins};
DifferentialControl control{leftMotor, rightMotor};
SimpleCar car{control};
VL53L0X sensor;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  while(!sensor.init());
  delay(5000);
  car.setSpeed(cruise_speed);
}
void loop() {
  if (sensor.readRangeSingleMillimeters() <= trigger_dist)
    car.setSpeed(0); // Halt
}
