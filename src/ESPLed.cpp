#include "ESPLed.h"


#ifdef ESP32

ESPLed::ESPLed(const gpio_num_t pin, const uint8_t channel, uint16_t freq_hz, const uint8_t off_state)
:
_PIN(pin),
_CHANNEL_32(channel,
_RESTING(off_state)
{

}

#else

ESPLed::ESPLed(const gpio_num_t pin, const uint8_t off_state)
:
_PIN(pin),
_RESTING(off_state)
{
  _initPin(_PIN);
}

#endif

ESPLed::~ESPLed() {
  if(_strategy != nullptr) delete _strategy;
  minBrightness(0);
  off();
}


ESPLed &ESPLed::maxBrightness(uint8_t percent) {
  _brightness.max = percent;
  return *this;
}

ESPLed &ESPLed::minBrightness(uint8_t percent){
  _brightness.min = percent;
  return *this;
} 

ESPLed &ESPLed::on(uint8_t percent) {
  _isOn = true;
  percent = constrain(percent, minBrightness(), maxBrightness());
#ifdef ESP32
  ledcWrite( channel(), _mapToAnalog(percent));
#else
  analogWrite( pin(), _mapToAnalog(percent) );
#endif
  return *this;
}

ESPLed &ESPLed::off() {
  _isOn = false;
#ifdef ESP32
  ledcWrite( channel(), _mapToAnalog(minBrightness()) );
#else
  analogWrite( pin(), _mapToAnalog(minBrightness()) );
#endif
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


uint16_t ESPLed::_mapToAnalog(uint8_t percent){

  /* First look up the value on a 10 bit scale */
  uint16_t ret = pgm_read_word(_brightnessLut + percent);

  /* Then map from 10 bit scale to PWM_RANGE */
  ret = map(ret, 0, 1023, 0, pwmRange());

  /* Account for LED where LOW = ON */
  return (_RESTING) ? ret : pwmRange() - ret;
  
}

void ESPLed::_initPin(gpio_num_t pin) {


  #ifndef ESP32
    pinMode(pin, OUTPUT);
  #endif


}











