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


void ESPPwmGpio::initialize() {


    #ifndef ESP32
        pinMode(_pin, OUTPUT);
    #endif

    // TODO add ESP32 stuff here
    digitalWrite(_pin, LOW);

}