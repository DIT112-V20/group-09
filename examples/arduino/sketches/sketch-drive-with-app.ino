//This sketch allows you to turn the led on and off on your smartcar.
//App that is used for basic communication with the car https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal&hl=sv 

#include "BluetoothSerial.h"
#include <Smartcar.h>
#include <Wire.h>

BluetoothSerial SerialBT;

int received;// received value will be stored in this variable
char receivedChar;// received value will be stored as CHAR in this variable

const char turnON ='a';
const char turnOFF ='b';
const char forward = 'f';
const char halt = 's';

BrushedMotor leftMotor{smartcarlib::pins::v2::leftMotorPins};
BrushedMotor rightMotor{smartcarlib::pins::v2::rightMotorPins};
DifferentialControl control{leftMotor, rightMotor};
SimpleCar car{control};

void setup() 
{
  //SerialBT writes on app
  //Serial writes on serial monitor

  Serial.begin(115200);
  Wire.begin();

  SerialBT.begin("Smartcar"); 
  SerialBT.println("To turn ON send: a"); 
  SerialBT.println("To turn OFF send: b");  
  Serial.println("To go forward send: f");  
  Serial.println("To stop send: s"); 
  Serial.println("The device started, now you can pair it with bluetooth!");
  Serial.println("To turn ON send: a");  
  Serial.println("To turn OFF send: b"); 
  Serial.println("To go forward send: f");  
  Serial.println("To stop send: s"); 
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() 
{
  receivedChar =(char)SerialBT.read();

  if (Serial.available()) 
  {
    SerialBT.write(Serial.read());
  }
  
  if (SerialBT.available()) 
  {
    switch(receivedChar) 
    {
      case 'a':
       SerialBT.println("LED ON:"); 
       Serial.println("LED ON:");
       digitalWrite(LED_BUILTIN, HIGH);
       break;

      case 'b':
       SerialBT.println("LED OFF:");
       Serial.println("LED OFF:");
       digitalWrite(LED_BUILTIN, LOW);
       break;

     case 'f':
      car.setSpeed(50);
      break;

     case 's':
      car.setSpeed(0);
      break;  
    }
  }
}
