#include "ESPBlink.h"
#include "ESPLedInterface.h"
#include "ESPLed.h"

ESPBlink &ESPBlink::interval(unsigned long ms) {
  assert( ms > 0 );
  ESPLedInterface::tickInterval(ms);
  return *this;   
}

ESPBlink &ESPBlink::duration(unsigned long ms) {

  /* Check precondition */
  assert( ms > 0 && ms < interval() );
  if(ms == 0) { Serial.println("BAD"); }
  Serial.println(ms);
  
  _duration_ms = ms;
  return *this;
}


 
void ESPBlink::_handleLed(ESPLed *const led) {
  Serial.println("Handling led");

  led->on();
  //if( !_blinkTick.active() )
    _blinkTick.once_ms(_duration_ms, _sOff, (void*)this);


  // _t.once_ms(_led.blinkDuration(), [this](){
  //   this->_led->off();
  // });



}

void ESPBlink::_sOff(void *blink) {
  ESPBlink *ptr = (ESPBlink*) blink;
  for(auto i : ptr->_leds) {
    i->off();
  }
}