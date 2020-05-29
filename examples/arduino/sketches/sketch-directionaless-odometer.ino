#include <Smartcar.h>

const unsigned long pulsesPerMeter = 600;

DirectionlessOdometer odometer(
smartcarlib::pins::v2::leftOdometerPin,[](){ odometer.update();}, pulsesPerMeter);

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    Serial.println(odometer.getDistance());
    delay(100);
}
