#include "ESPLed.h"


#ifdef ESP32

ESPLed::ESPLed(const gpio_num_t pin, const uint8_t channel, uint16_t freq_hz, const uint8_t off_state)
:
_gpio(pin, channel, freq_hz)
_highIsOn(off_state != HIGH ? true : false)
{
	_gpio.initialize();
} 

#else

ESPLed::ESPLed(const gpio_num_t pin, const uint8_t off_state)
:
_gpio(pin, off_state == LOW),
_highIsOn(off_state != HIGH ? true : false)
{
	_gpio.initialize();
}

#endif

ESPLed::~ESPLed() {
	stop();
	setMinBrightness(0);
	off();
}


ESPLed &ESPLed::setMaxBrightness(uint8_t percent) {
	_brightnessRange.setMaxBrightnessPercent(percent);
	return *this;
}

ESPLed &ESPLed::setMinBrightness(uint8_t percent){
	_brightnessRange.setMinBrightnessPercent(percent);
	return *this;
} 




ESPLed &ESPLed::on(uint8_t percent) {

	// Force percent to be within min/max
	percent = constrain(percent, getMinBrightness(), getMaxBrightness());

	// Map the percent to an antilog analog value
	uint16_t analog_value = ESPLedBrightness::percentToAnalog(percent);

	// Generate PWM, this method compensates for off_state
	_gpio.analogWrite(analog_value);
	Serial.println("wrote");
	_isOn = true;
	return *this;
}

ESPLed &ESPLed::off() {
	this->on(0); 
	_isOn = false;
	return *this;
}



ESPLed &ESPLed::toggle(uint8_t power) { 
	isOn() ? off() : on(power); 
	return *this;
} 

ESPLed &ESPLed::setMode(ESPLedInterface &s) {
	if( isActive() ) stop();
	_strategy = &s;
	return *this;
}

ESPLed &ESPLed::start() {
	if(_strategy != nullptr){
		_strategy->start(*this);
	}
	return *this;
}

ESPLed &ESPLed::stop() {
	if( isActive() ) {
		_strategy->stop(*this);
	}
	return *this;
}

bool ESPLed::isActive() const {
	return (_strategy != nullptr) && ( _strategy->isStarted() ); 
}














/* Basic getters */
gpio_num_t ESPLed::getPin() const { return _gpio.pin(); }
uint8_t ESPLed::getMaxBrightness() const { return _brightnessRange.getMaxBrightnessPercent(); }
uint8_t ESPLed::getMinBrightness() const { return _brightnessRange.getMinBrightnessPercent(); }
ESPLedInterface &ESPLed::getMode() const { return *_strategy; }
bool ESPLed::isOn() const { return _isOn; }
ESPLed::operator bool() const { return _isOn; }
bool ESPLed::highIsOn() const { return _highIsOn; }