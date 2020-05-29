//This sketch allows you to turn the led on and off on your smartcar.
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

int received;// received value will be stored in this variable
char receivedChar;// received value will be stored as CHAR in this variable

const char turnON ='a';
const char turnOFF ='b';

void setup() 
{
  //SerialBT writes on app
  //Serial writes on serial monitor
  
  Serial.begin(115200);
  SerialBT.begin("Smartcar"); 
  SerialBT.println("To turn ON send: a"); 
  SerialBT.println("To turn OFF send: b");  
  Serial.println("The device started, now you can pair it with bluetooth!");
  Serial.println("To turn ON send: a");  
  Serial.println("To turn OFF send: b"); 
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
    
      if(receivedChar == turnON)
      {
         SerialBT.println("LED ON:"); 
         Serial.println("LED ON:");
         digitalWrite(LED_BUILTIN, HIGH);
      }
      
    if(receivedChar == turnOFF)
    {
     SerialBT.println("LED OFF:");
     Serial.println("LED OFF:");
     digitalWrite(LED_BUILTIN, LOW);
    }    
  }
}
