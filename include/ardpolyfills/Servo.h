/*
 *  Servo.h
 *  Copyright 2020 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#ifndef Servo_h
#define Servo_h

#include <cstdint>
#include "SMCE__dll.hxx"

enum SMCE__Boundary{ MIN_PULSE_WIDTH = 1000, MAX_PULSE_WIDTH = 2000, DEFAULT_PULSE_WIDTH = 1500 };

class SMCE__DLL_RT_API Servo {
  public:
    constexpr Servo() = default; 
   
    /**
    * Attach the Servo variable to a pin.
    **/
    void attach(int pin);
  
    /**
    * Attaches a pin to attached_pin with a set min and max for the pulse_width in degrees
    **/
    void attach(int pin, int min, int max);
   
    /**
    * Detaches the pin to default value 
    **/
    void detach();
    
    /**
    * Writes a value that controls the shaft accodingly
    **/
    void write(int value);

    /**
    * Writes the value to the servo in microseconds  ----->>>>
    **/
    void writeMicroseconds(int value);

    /**
    * Reads the angle of the servo, that is it returns the attached pins value
    **/
    int read();

    /**
    * Returns true if attached_pin has beens set
    **/
    bool attached() const noexcept;

  private:
    /// Contains an attached pin, default value is -1
    int attached_pin = -1;
    /// Contains the min value for min pulse width
    int8_t min = 0;
    /// Contains the max value for max pulse width
    int8_t max = 0;
};

#endif // Servo_h
