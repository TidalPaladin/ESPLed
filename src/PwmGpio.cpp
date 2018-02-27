#include "PwmGpio.h"





ESPPwmGpio::~ESPPwmGpio(){
    this->analogWrite(0);
}

void ESPPwmGpio::analogWrite(uint16_t analog_value) {

    analog_value = _onWhenHigh ? analog_value : pwmRange() - analog_value;

#ifdef ESP32
    ledcWrite( channel(), analog_value);
#else
    ::analogWrite( pin(), analog_value );
#endif


}


void ESPPwmGpio::initialize() {

    #ifndef ESP32
        pinMode(_pin, OUTPUT);
    #else
        ledcSetup(_channel, _frequency, _resolution);
        ledcAttachPin(_pin);
    #endif
  
    // Start in LOW state
    analogWrite(0);
   
}