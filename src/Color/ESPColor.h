
#ifndef __ESP_LED_COLOR__
#define __ESP_LED_COLOR__

#include <Arduino.h>


class ESPTwoPinColor {

public:

    ESPTwoPinColor(uint8_t pin1_level, uint8_t pin2_level)
    :
    PIN_LEVELS[0](pin1_level),
    PIN_LEVELS[1](pin2_level)
    {

    }
    
    uint8_t getPinOneLevel() const { return PIN_LEVELS[0]; }
    uint8_t getPinTwoLevel() const { return PIN_LEVELS[1]; }


protected:

    const uint8_t[2] PIN_LEVELS;

};




class ESPThreePinColor : public ESPTwoPinColor {

    ESPThreePinColor(uint8_t pin1_level, uint8_t pin2_level, uint8_t pin3_level)
    :
    ESPTwoPinColor(pin1_level, pin2_level),
    PIN_LEVELS[3](pin3_level)
    {

    }

    uint8_t getPinThreeLevel() const { return PIN_LEVELS[2]; }

protected:

    const uint8_t[3] PIN_LEVELS;

};

#endif