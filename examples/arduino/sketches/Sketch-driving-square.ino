#include <Smartcar.h>

const unsigned long pulsesPerMeter = 600;
const int degreesToTurn = 90;
const int distance = 60;
GY50 gyro(37);

BrushedMotor leftMotor{smartcarlib::pins::v2::leftMotorPins};
BrushedMotor rightMotor{smartcarlib::pins::v2::rightMotorPins};
DifferentialControl control{leftMotor, rightMotor};

DirectionlessOdometer leftOdometer(
    smartcarlib::pins::v2::leftOdometerPin, []() { leftOdometer.update(); }, pulsesPerMeter);
DirectionlessOdometer rightOdometer(
    smartcarlib::pins::v2::rightOdometerPin, []() { rightOdometer.update(); }, pulsesPerMeter);

SmartCar car{control, gyro, leftOdometer, rightOdometer};

void setup() 
{
  Serial.begin(9600);
  go(distance);
  rotate(degreesToTurn); 
  go(distance);
  rotate(degreesToTurn);
  go(distance);
  rotate(degreesToTurn);
  go(distance);
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

  const auto initialHeading    = car.getHeading();
  bool hasReachedTargetDegrees = false;
  while (!hasReachedTargetDegrees)
  {
      car.update();
      auto currentHeading = car.getHeading();
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

void go(long centimeters)
{
  car.setAngle(0);
  car.setSpeed(30);

    long initialDistance          = car.getDistance();
    bool hasReachedTargetDistance = false;
    while (!hasReachedTargetDistance)
    {
        car.update();
        auto currentDistance   = car.getDistance();
        auto travelledDistance = initialDistance > currentDistance
                                     ? initialDistance - currentDistance
                                     : currentDistance - initialDistance;
        hasReachedTargetDistance
            = travelledDistance >= smartcarlib::utils::getAbsolute(centimeters);
    }
    car.setSpeed(0);
}



 
