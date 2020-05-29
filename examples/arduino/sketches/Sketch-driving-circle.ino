#include <Smartcar.h>

const int degreesToTurn = 45;

BrushedMotor leftMotor{smartcarlib::pins::v2::leftMotorPins};
BrushedMotor rightMotor{smartcarlib::pins::v2::rightMotorPins};
DifferentialControl control{leftMotor, rightMotor};

SimpleCar car{control};

void setup() 
{
  Serial.begin(9600);
  circle(degreesToTurn);
}

void loop() 
{   
}

void circle(int degrees)
{
  car.setAngle(degrees);
  car.setSpeed(50);
}
