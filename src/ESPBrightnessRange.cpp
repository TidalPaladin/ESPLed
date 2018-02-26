#include "ESPBrightnessRange.h"


ESPLedBrightness &ESPLedBrightness::maxBrightnessPercent(uint8_t percent) {
    if( percent <= minBrightnessPercent() )
        percent = minBrightnessPercent() + 1;

    _maxBrightnessPercent = percent;
    return *this;
}

ESPLedBrightness &ESPLedBrightness::minBrightnessPercent(uint8_t percent) {
    if( percent >= maxBrightnessPercent() )
        percent = maxBrightnessPercent() - 1;

    _minBrightnessPercent = percent;
    return *this;
}


uint16_t ESPLedBrightness::percentToAnalog(uint8_t percent) {

    assert(percent >= 0 && percent <= 100);
    if(percent == 0) return 0;

    // TODO figure out a better way to store table data & decouple PWMRANGE from table entry
    uint16_t analog_value = pgm_read_word(_brightnessLut + percent);
    return map( analog_value, 0, 1023, 0, ESPPwmGpio::pwmRange() ); 

  
}

 