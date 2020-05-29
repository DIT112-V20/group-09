#include<Smartcar.h>

const int GYROSCOPE_OFFSET = 37;

GY50 gyro(GYROSCOPE_OFFSET);
void setup() 
{
  Serial.begin(9600);
  delay(1500);
}

void loop() 
{
   gyro.update();
    Serial.println(gyro.getHeading());
}
