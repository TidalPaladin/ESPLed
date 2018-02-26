#include "PwmGpio.h"





ESPPwmGpio::~ESPPwmGpio(){
    this->analogWrite(0);
}

void ESPPwmGpio::analogWrite(uint16_t analog_value) {

#ifdef ESP32
    ledcWrite( channel(), analog_value);
#else
    ::analogWrite( pin(), analog_value );
#endif


}

void ESPPwmGpio::analogWrite(uint16_t analog_value) {

    // Invert duty cycle if needed
    analog_value = _onWhenHigh ? analog_value : pwmRange() - analog_value;

    #ifndef ESP32
        digitalWrite(_pin, analog_value);
    #else 
        ledcWrite(_pin, analog_value);
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