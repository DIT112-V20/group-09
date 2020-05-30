#include <Smartcar.h>
#include <VL53L0X.h>
#include <Wire.h>

const int degreesToTurn = 90;
constexpr unsigned trigger_dist = 300;
GY50 gyro(37);
VL53L0X sensor;

BrushedMotor leftMotor{smartcarlib::pins::v2::leftMotorPins};
BrushedMotor rightMotor{smartcarlib::pins::v2::rightMotorPins};
AckermanControl control{leftMotor, rightMotor};

SimpleCar car{control};

void setup() 
{
  Serial.begin(9600);
  Wire.begin();
  while(!sensor.init());
  go();
  rotate(degreesToTurn); 
  go();
  rotate(degreesToTurn);
  go();
  rotate(degreesToTurn);
  go();
  rotate(degreesToTurn);
}

void loop() 
{
}

void rotate(int degrees)
{
  car.setAngle(degrees);
  car.setSpeed(30);
  degrees %= 360; 

  const auto initialHeading    = gyro.getHeading();
  bool hasReachedTargetDegrees = false;
  while (!hasReachedTargetDegrees)
  {
      gyro.update();
      auto currentHeading = gyro.getHeading();
      if (degrees > 0 && currentHeading < initialHeading)
      {
          currentHeading += 360;
      }
      int degreesTurnedSoFar  = initialHeading - currentHeading;
      hasReachedTargetDegrees = smartcarlib::utils::getAbsolute(degreesTurnedSoFar)
                                >= smartcarlib::utils::getAbsolute(degrees);
  }
  car.setSpeed(0);
}

void go()
{
  car.setAngle(0);
  car.setSpeed(30);

  bool hasReachedTargetDistance = false;
  while (!hasReachedTargetDistance)
  {
      if(sensor.readRangeSingleMillimeters() > trigger_dist)
      {
        hasReachedTargetDistance = true;
      }
  }
  car.setSpeed(0);
}



 
