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

void ESPBlink::_loop() {
  
  for(auto i : _leds) {
    i->on();
  }

  //_blinkTick.once_ms(_duration_ms, )
  // _t.once_ms(_led.blinkDuration(), [this](){
  //   this->_led->off();
  // });


#ifdef ESP32
  _preventTaskEnd();
#endif


}
