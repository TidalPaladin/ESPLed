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
_gpio(pin),
_highIsOn(off_state != HIGH ? true : false)
{
  _gpio.initialize();
}

#endif

ESPLed::~ESPLed() {
  if(_strategy != nullptr) delete _strategy;
  minBrightness(0);
  off();
}


ESPLed &ESPLed::maxBrightness(uint8_t percent) {
  _brightnessRange.maxBrightnessPercent(percent);
  return *this;
}

ESPLed &ESPLed::minBrightness(uint8_t percent){
  _brightnessRange.minBrightnessPercent(percent);
  return *this;
} 



ESPLed &ESPLed::on(uint8_t percent) {
  percent = constrain(percent, minBrightness(), maxBrightness());
  uint16_t analog_value = ESPLedBrightness::percentToAnalog(percent);
  _gpio.analogWrite(analog_value);

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

  if(_strategy != nullptr) { 
    stop();
  }
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
  if(_strategy != nullptr){
    _strategy->stop(*this);
  }
  return *this;
}
















