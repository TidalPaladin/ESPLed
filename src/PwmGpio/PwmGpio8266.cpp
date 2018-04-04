#ifdef ESP8266
#include "PwmGpio8266.h"

ESPPwmGpio::~ESPPwmGpio() { this->analogWrite(0); }

ESPPwmGpio ESPPwmGpio::onWhenHigh(bool value) {
	_onWhenHigh = value;
	return *this;
}
bool ESPPwmGpio::onWhenHigh() const { return _onWhenHigh; }
gpio_num_t ESPPwmGpio::pin() const { return _pin; }
uint32_t ESPPwmGpio::pwmRange() { return ::pwm_range; }
uint32_t ESPPwmGpio::pwmFrequency() { return ::pwm_freq; }
uint8_t ESPPwmGpio::channel() const { return _channel; }
unsigned long ESPPwmGpio::frequency() const { return _frequency; }

void ESPPwmGpio::analogWrite(volatile uint16_t analog_value) {

	if (analog_value > pwmRange()) analog_value = pwmRange();
	analog_value = _onWhenHigh ? analog_value : pwmRange() - analog_value;

	::analogWrite(pin(), 0);
	::analogWrite(pin(), analog_value);
	// yield();
	// ESP.wdtFeed();
}

void ESPPwmGpio::initialize() {
	pinMode(_pin, OUTPUT);

	// Start in LOW state
	analogWrite(0);
}
#endif