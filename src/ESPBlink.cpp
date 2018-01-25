#include "ESPBlink.h"
#include "ESPLedInterface.h"
#include "ESPLed.h"

ESPBlink &ESPBlink::interval(unsigned long ms) {
  _interval_ms = ms;
  return *this;   
}

ESPBlink &ESPBlink::duration(unsigned long ms) {
  _duration_ms = ms;
  return *this;
}



void ESPBlink::_handleLed(ESPLed *const led) {
  
  led->on();
  _blinkTick.once_ms(_duration_ms, _sOff, (void*)this);
  // _t.once_ms(_led.blinkDuration(), [this](){
  //   this->_led->off();
  // });


#ifdef ESP32
  _preventTaskEnd();
#endif


}

void ESPBlink::_sOff(void *blink) {
  ESPBlink *ptr = (ESPBlink*) blink;
  for(auto i : ptr->_leds) {
    i->off();
  }
}