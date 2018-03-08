#include "ESPBrightnessRange.h"


ESPLedBrightness &ESPLedBrightness::setMaxBrightnessPercent(uint8_t percent) {
	
	_maxBrightnessPercent = percent;

	if( percent <= _minBrightnessPercent )
		_minBrightnessPercent = percent - 1;

	return *this;
}

ESPLedBrightness &ESPLedBrightness::setMinBrightnessPercent(uint8_t percent) {
	

	_minBrightnessPercent = percent;

	if( percent >= _maxBrightnessPercent )
		_maxBrightnessPercent = percent + 1;

	return *this;
}


uint16_t ESPLedBrightness::percentToAnalog(uint8_t percent) {

	if(percent > 100)
		percent = 100;

	if(percent == 0) return 0;

	// TODO figure out a better way to store table data & decouple PWMRANGE from table entry
	// uint16_t analog_value = pgm_read_word(_brightnessLut + percent);
	// return map( analog_value, 0, 1023, 0, ESPPwmGpio::pwmRange() ); 
	uint16_t ret = percentToAnalogExact(percent);
	return ret;
  
}

uint16_t ESPLedBrightness::percentToAnalogExact(const uint8_t percent) {

	//return pow( (float)percent / 100, 3 ) * PWMRANGE + 0.5;
	//return percent * percent * percent * PWMRANGE / (100*100*100) + 0.5;
	return percent * percent * percent * ESPPwmGpio::pwmRange() / (1000000);


}

 