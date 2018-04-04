#include "ESPBrightnessRange.h"

ESPLedBrightness::ESPLedBrightness(uint8_t min_percent, uint8_t max_percent)
	: _minBrightnessPercent(min_percent), _maxBrightnessPercent(max_percent) {
	assert(min_percent < max_percent && max_percent <= 100);
}

ESPLedBrightness::ESPLedBrightness() : ESPLedBrightness(0, 100) {}

ESPLedBrightness &ESPLedBrightness::setMaxBrightnessPercent(uint8_t percent) {

	_maxBrightnessPercent = percent;

	if (percent <= _minBrightnessPercent) _minBrightnessPercent = percent - 1;

	return *this;
}

ESPLedBrightness &ESPLedBrightness::setMinBrightnessPercent(uint8_t percent) {

	_minBrightnessPercent = percent;

	if (percent >= _maxBrightnessPercent) _maxBrightnessPercent = percent + 1;

	return *this;
}

uint16_t ESPLedBrightness::percentToAnalog(uint8_t percent,
										   uint8_t resolution_bits) {

	if (percent == 0)
		return 0;
	else if (percent > 100)
		percent = 100;

	return pow((double)percent / 100, 3);
}
