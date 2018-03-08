#include "PwmGpio.h"





ESPPwmGpio::~ESPPwmGpio(){
    this->analogWrite(0);
}

void ESPPwmGpio::analogWrite(volatile uint16_t analog_value) {

    if(analog_value > pwmRange()) analog_value = pwmRange();
    analog_value = _onWhenHigh ? analog_value : pwmRange() - analog_value;

#ifdef ESP32
    ledcWrite( channel(), analog_value);
#else
    ::analogWrite( pin(), 0);
    ::analogWrite( pin(), analog_value );
    // yield();
    // ESP.wdtFeed();
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