#include <Smartcar.h>
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;
void setup()
{
  Serial.begin(9600);
  Wire.begin();
  sensor.setTimeout(500);
  while(!sensor.init());
}

void loop()
{
  Serial.print(sensor.readRangeSingleMillimeters());
  delay(500);
  Serial.println();
}
