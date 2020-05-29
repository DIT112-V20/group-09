#include <Smartcar.h>

constunsignedlong pulsesPerMeter =600;

DirectionalOdometer odometer( 
  smartcarlib::pins::v2::leftOdometerPins,
  [](){ odometer.update();},
      pulsesPerMeter);

void setup()
{    
  Serial.begin(9600);
}

void loop()
{    
  Serial.println(odometer.getDistance());    
  delay(100);
}
