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

	assert(percent >= 0 && percent <= 100);
	if(percent == 0) return 0;

	// TODO figure out a better way to store table data & decouple PWMRANGE from table entry
	// uint16_t analog_value = pgm_read_word(_brightnessLut + percent);
	// return map( analog_value, 0, 1023, 0, ESPPwmGpio::pwmRange() ); 
	return percentToAnalogExact(percent);
  
}

constexpr uint16_t ESPLedBrightness::percentToAnalogExact(const uint8_t percent) {
	return (uint16_t)(
		pow(
			(float)percent / (float)__ESP_BRIGHTNESS_RANGE_MAX_IN__, __ESP_BRIGHTNESS_RANGE_GAMMA__
		) * 
		__ESP_BRIGHTNESS_RANGE_MAX_OUT__ 
		+ 0.5
		)
	;
}

 