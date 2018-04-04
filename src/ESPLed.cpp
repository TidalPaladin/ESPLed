#include "ESPLed.h"

ESPLedBase::~ESPLedBase() {
	stop();
	setMinBrightness(0);
	off();
}

ESPLedBase& ESPLedBase::setMaxBrightness(uint8_t percent) {
	_brightnessRange.setMaxBrightnessPercent(percent);
	return *this;
}

ESPLedBase& ESPLedBase::setMinBrightness(uint8_t percent) {
	_brightnessRange.setMinBrightnessPercent(percent);
	return *this;
}

ESPLedBase& ESPLedBase::on(uint8_t percent) {
	_isOn = percent > getMinBrightness();

	// Force percent to be within min/max
	percent = constrain(percent, getMinBrightness(), getMaxBrightness());

	// Map the percent to an antilog analog value
	// uint16_t analog_value = ESPLedBrightness::percentToAnalog(percent);

	return *this;
}

ESPLedBase& ESPLedBase::off() {
	return this->on(0);
}

ESPLedBase& ESPLedBase::toggle(uint8_t power) {
	isOn() ? off() : on(power);
	return *this;
}

ESPLedBase& ESPLedBase::setMode(ESPLedInterface& s) {
	if (isActive())
		stop();
	_strategy = &s;
	return *this;
}

ESPLedBase& ESPLedBase::start() {
	if (_strategy != nullptr) {
		_strategy->start(*this);
	}
	return *this;
}

ESPLedBase& ESPLedBase::stop() {
	if (isActive()) {
		_strategy->stop(*this);
	}
	return *this;
}

bool ESPLedBase::isActive() const {
	return (_strategy != nullptr) && (_strategy->isStarted());
}

uint8_t ESPLedBase::getMaxBrightness() const {
	return _brightnessRange.getMaxBrightnessPercent();
}

uint8_t ESPLedBase::getMinBrightness() const {
	return _brightnessRange.getMinBrightnessPercent();
}

ESPLedInterface* ESPLedBase::getMode() const {
	return _strategy;
}

bool ESPLedBase::isOn() const {
	return _isOn;
}

ESPLedBase::operator bool() const {
	return _isOn;
}

bool ESPLedBase::highIsOn() const {
	return _highIsOn;
}