#include <Smartcar.h>
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;
void setup()
{
  Serial.begin(9600);
  Wire.begin();

  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
}

void loop()
{
  Serial.print(sensor.readRangeSingleMillimeters());
  Serial.println();
}
