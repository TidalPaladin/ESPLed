#include "ESP8266Led.h"

ESPLed::ESPLed(uint8_t channel, double freq, uint8_t resolution_bits) {
	// TODO implement
}

ESPLed::ESPLed() {
	// TODO implement
}

gpio_num_t ESPLed::getPin() const {
	// Todo implement
	return 0;
}

ESPLed& ESPLed::on(uint8_t percent = 100) {
	// Todo implement
	return *this;
}

ESPLed& ESPLed::off() {
	// Todo implement
	return *this;
}

ESPLed& ESPLed::toggle(uint8_t percent = 100) {
	// Todo implement
	return *this;
}

uint8_t ESPLed::findNextDefaultChannel() {
	// Todo implement
	return 0;
}